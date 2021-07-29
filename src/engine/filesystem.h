#pragma once

#include <stdio.h>
#include "engine/common.h"

#define CD_MAX_FILENAME 16
#define CD_MAX_PATH (128 + CD_MAX_FILENAME)

typedef struct fs_file_s fs_file_t;

void cd_init(void);

fs_file_t *fs_fopen(const char *fname, const int reopen);
int fs_fexists(const char *fname);
void fs_fclose(fs_file_t *f);
int fs_fread(void *ptr, int size, int num, fs_file_t *f);
void fs_fread_or_die(void *ptr, int size, int num, fs_file_t *f);
int fs_fseek(fs_file_t *f, int ofs, int whence);
int fs_ftell(fs_file_t *f);
int fs_fsize(fs_file_t *f);
int fs_feof(fs_file_t *f);
u8 fs_fread_u8(fs_file_t *f);
u16 fs_fread_u16(fs_file_t *f);
u32 fs_fread_u32(fs_file_t *f);

int fs_scan_dir(const char *dir, char out[][CD_MAX_FILENAME], const char *filter);
