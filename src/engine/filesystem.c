#include "engine/filesystem.h"

#include <psxcd.h>
#include <psxgpu.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine/common.h"

#define SECSIZE 2048
#define BUFSECS 4
#define BUFSIZE (BUFSECS * SECSIZE)
#define MAX_FHANDLES 1

static u_long cdmode = CdlModeSpeed;

struct fs_file_s {
  char fname[CD_MAX_PATH];
  CdlFILE cdf;
  int secstart, secend, seccur;
  int fp, bufp;
  int bufleft;
  u8 buf[BUFSIZE];
};

// lmao 1handle
static fs_file_t fhandle;
static int num_fhandles = 0;

void cd_init(void) {
  // psn00b's CdInit always returns 1, so we'll have to check for success below
  CdInit();

  // see if there was a CD in the drive during init, die if there wasn't
  if (CdSync(1, NULL) == CdlDiskError)
    panic("bad CD or no CD in drive");

  // look alive
  CdControl(CdlNop, 0, 0);
  CdStatus();

  // set hispeed mode
  CdControlB(CdlSetmode, (u_char *)&cdmode, 0);
  VSync(3); // have to do this to not explode the drive apparently
}

fs_file_t *fs_fopen(const char *fname, const int reopen) {
  // check if the same file was just open and return it if allowed
  if (reopen && !strncmp(fhandle.fname, fname, sizeof(fhandle.fname))) {
    num_fhandles++;
    return &fhandle;
  }

  if (num_fhandles >= MAX_FHANDLES) {
    printf("fs_fopen(%s): too many file handles\n", fname);
    return NULL;
  }

  fs_file_t *f = &fhandle;
  memset(f, 0, sizeof(*f));

  if (CdSearchFile(&f->cdf, fname) == NULL) {
    printf("fs_fopen(%s): file not found\n", fname);
    return NULL;
  }

  // read first sector of the file
  CdControl(CdlSetloc, (u_char *)&f->cdf.pos, 0);
  CdRead(BUFSECS, (u_long *)f->buf, CdlModeSpeed);
  CdReadSync(0, NULL);

  // set fp and shit
  f->secstart = CdPosToInt(&f->cdf.pos);
  f->seccur = f->secstart;
  f->secend = f->secstart + (f->cdf.size + SECSIZE - 1) / SECSIZE;
  f->fp = 0;
  f->bufp = 0;
  f->bufleft = (f->cdf.size >= BUFSIZE) ? BUFSIZE : f->cdf.size;
  strncpy(fhandle.fname, fname, sizeof(fhandle.fname) - 1);

  num_fhandles++;
  printf("fs_fopen(%s): size %u bufleft %d secs %d %d\n", fname, f->cdf.size,
    f->bufleft, f->secstart, f->secend);

  return f;
}

int fs_fexists(const char *fname) {
  CdlFILE cdf;
  if (CdSearchFile(&cdf, (char *)fname) == NULL) {
    printf("fs_fexists(%s): file not found\n", fname);
    return FALSE;
  }
  return TRUE;
}

void fs_fclose(fs_file_t *f) {
  if (!f) return;
  num_fhandles--;
}

int fs_fread(void *ptr, int size, int num, fs_file_t *f) {
  int rx, rdbuf;
  int fleft;
  CdlLOC pos;

  if (!f || !ptr) return -1;
  if (!size) return 0;

  size *= num;
  rx = 0;

  while (size) {
    // first empty the buffer
    rdbuf = (size > f->bufleft) ? f->bufleft : size;
    memcpy(ptr, f->buf + f->bufp, rdbuf);
    rx += rdbuf;
    ptr += rdbuf;
    f->fp += rdbuf;
    f->bufp += rdbuf;
    f->bufleft -= rdbuf;
    size -= rdbuf;

    // if we went over, load next sector
    if (f->bufleft == 0) {
      f->seccur += BUFSECS;
      // check if we have reached the end
      if (f->seccur >= f->secend) return rx;
      // looks like you need to seek every time when you use CdRead
      CdIntToPos(f->seccur, &pos);
      CdControl(CdlSetloc, (u_char *)&pos, 0);
      CdRead(BUFSECS, (u_long *)f->buf, CdlModeSpeed);
      CdReadSync(0, 0);
      fleft = f->cdf.size - f->fp;
      f->bufleft = (fleft >= BUFSIZE) ? BUFSIZE : fleft;
      f->bufp = 0;
    }
  }

  return rx;
}

void fs_fread_or_die(void *ptr, int size, int num, fs_file_t *f) {
  if (fs_fread(ptr, size, num, f) < 0)
    panic("fs_fread_or_die(%.16s, %d, %d): fucking died", f->cdf.name, size, num);
}

int fs_fseek(fs_file_t *f, int ofs, int whence) {
  int fsec, bofs;
  CdlLOC pos;

  if (!f) return -1;

  if (whence == SEEK_CUR) ofs = f->fp + ofs;

  if (f->fp == ofs) return 0;

  fsec = f->secstart + (ofs / BUFSIZE) * BUFSECS;
  bofs = ofs % BUFSIZE;

  // fuck SEEK_END, it's only used to get file length here

  if (fsec != f->seccur) {
    // sector changed; seek to new one and buffer it
    CdIntToPos(fsec, &pos);
    CdControl(CdlSetloc, (u_char *)&pos, 0);
    CdRead(BUFSECS, (u_long *)f->buf, CdlModeSpeed);
    CdReadSync(0, 0);
    f->seccur = fsec;
    f->bufp = -1; // hack: see below
  }

  if (bofs != f->bufp) {
    // buffer offset changed (or new sector loaded); reset pointers
    f->bufp = bofs;
    f->bufleft = BUFSIZE - bofs;
    if (f->bufleft < 0) f->bufleft = 0;
  }

  f->fp = ofs;

  return 0;
}

int fs_ftell(fs_file_t *f) {
  if (!f) return -1;
  return f->fp;
}

int fs_fsize(fs_file_t *f) {
  if (!f) return -1;
  return f->cdf.size;
}

int fs_feof(fs_file_t *f) {
  if (!f) return -1;
  return (f->seccur >= f->secend);
}

u8 fs_fread_u8(fs_file_t *f) {
  u8 res = 0;
  fs_fread_or_die(&res, 1, 1, f);
  return res;
}

u16 fs_fread_u16(fs_file_t *f) {
  u16 res = 0;
  fs_fread_or_die(&res, 2, 1, f);
  return res;
}

u32 fs_fread_u32(fs_file_t *f) {
  u32 res = 0;
  fs_fread_or_die(&res, 4, 1, f);
  return res;
}

int fs_scan_dir(const char *dir, char out[][CD_MAX_FILENAME], const char *filter) {
  CdlFILE cdf;
  CdlDIR *cddir = CdOpenDir(dir);
  if (!cddir) return -1;
  int n = 0;
  while (CdReadDir(cddir, &cdf)) {
    if (cdf.name[0] && cdf.name[0] != '.' &&
        (!filter || strstr(cdf.name, filter))) {
      strncpy(out[n], cdf.name, CD_MAX_FILENAME);
      ++n;
    }
  }
  return n;
}
