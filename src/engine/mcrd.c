#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <libapi.h>
#include <libetc.h>

#include "engine/common.h"
#include "engine/timer.h"
#include "engine/mcrd.h"

#define MCRD_SAVE_MAGIC "CSS\x01"
#define MCRD_SONY_MAGIC "SC"

#define MCRD_NUM_EVSPEC 4
#define MCRD_NUM_EVDESC 2
#define MCRD_EV_SW 0
#define MCRD_EV_HW 1

// event index -> event spec id
static const u32 mcrd_ev_spectab[MCRD_NUM_EVSPEC] = { 
  EvSpIOE, EvSpERROR, EvSpTIMOUT, EvSpNEW
};

// event descriptor index -> event descriptor id
static const u32 mcrd_ev_desctab[MCRD_NUM_EVSPEC] = {
  SwCARD, HwCARD
};

// all the memcard events
static u32 mcrd_ev[MCRD_NUM_EVDESC][MCRD_NUM_EVSPEC];

// save file we're currently working with
static mcrd_save_t mcrd_save_hdr;
static int mcrd_save_fd = -1;
// memcard we're currently working with
static int mcrd_chan = -1;

static u16 save_icon_clut[] = {
  0x0000, 0x151F, 0x6FBC, 0x0FF3,
  0x0040, 0x5E91, 0x332D, 0x3881,
};

static u8 save_icon_data[] = {
#include "saveicon.inc"
};

static inline char *mcrd_devname(const int chan) {
  static char buf[6] = "bu00:";
  buf[2] = '0' + (chan / 16); // port
  buf[3] = '0' + (chan % 4); // card
  return buf;
}

static u32 mcrd_wait_event(const int desc_num) {
  int timeout = 0xFFFFFF;

  while (timeout--) {
    for (int i = 0; i < MCRD_NUM_EVSPEC; ++i) {
      if (TestEvent(mcrd_ev[desc_num][i]))
        return mcrd_ev_spectab[i];
    }
  }

  // might as well consider it timed out
  return EvSpTIMOUT;
}

static inline void mcrd_clear_events(const int desc_num) {
  for (int i = 0; i < MCRD_NUM_EVSPEC; ++i)
    TestEvent(mcrd_ev[desc_num][i]);
}

static mcrd_result_t mcrd_card_check(const int chan) {
  _card_info(chan);

  mcrd_result_t ret;

  switch (mcrd_wait_event(MCRD_EV_SW)) {
    case EvSpNEW:
      // freshly connected card; do dummy write
      mcrd_clear_events(MCRD_EV_HW);
      _card_clear(chan);
      mcrd_wait_event(MCRD_EV_HW);
      /* fallthrough */
    case EvSpIOE:
      ret = MCRD_SUCCESS;
      break;
    case EvSpTIMOUT:
      ret = MCRD_NO_CARD;
      break;
    default:
      ret = MCRD_ERROR;
      break;
  }

  return ret;
}

void mcrd_init(void) {
  VSync(0);

  EnterCriticalSection();

  InitCARD(TRUE);
  StartCARD();
  ChangeClearPAD(0);
  _bu_init();

  for (int i = 0; i < MCRD_NUM_EVDESC; ++i) {
    for (int j = 0; j < MCRD_NUM_EVSPEC; ++j) {
      mcrd_ev[i][j] = OpenEvent(mcrd_ev_desctab[i], mcrd_ev_spectab[j], EvMdNOINTR, NULL);
      if (mcrd_ev[i][j] == -1)
        PANIC("mcrd_init(): could not open event (%d, %d)\ndesc=%08x spec=%08x", i, j, mcrd_ev_desctab[i], mcrd_ev_spectab[j]);
      DisableEvent(mcrd_ev[i][j]);
    }
  }

  ExitCriticalSection();

  printf("mcrd_init(): memcard initialized\n");
}

void mcrd_start(void) {
  // tick music in the timer callback because all memcard processing is synchronous
  timer_set_callback(timer_cb_music);

  mcrd_clear_events(MCRD_EV_HW);
  mcrd_clear_events(MCRD_EV_SW);

  EnterCriticalSection();
  for (int i = 0; i < MCRD_NUM_EVDESC; ++i) {
    for (int j = 0; j < MCRD_NUM_EVSPEC; ++j)
      EnableEvent(mcrd_ev[i][j]);
  }
  ExitCriticalSection();

  // wait a bit before letting the user do shit
  VSync(3);

  mcrd_save_hdr.id[0] = 0;
  mcrd_chan = -1;
  mcrd_save_fd = -1;

  printf("mcrd_start(): memcard processing started\n");
}

void mcrd_stop(void) {
  mcrd_card_close();

  EnterCriticalSection();
  for (int i = 0; i < MCRD_NUM_EVDESC; ++i) {
    for (int j = 0; j < MCRD_NUM_EVSPEC; ++j)
      DisableEvent(mcrd_ev[i][j]);
  }
  ExitCriticalSection();

  // reset timer back to normal callback
  timer_set_callback(timer_cb_ticker);

  printf("mcrd_stop(): memcard processing stopped\n");
}

int mcrd_cards_available(mcrd_id_t *out_cards) {
  int ret = 0;

  for (int port = 0; port < MCRD_NUM_PORTS; ++port) {
    for (int card = 0; card < MCRD_CARDS_PER_PORT; ++card) {
      const int chan = port * 16 + card;
      mcrd_clear_events(MCRD_EV_SW);
      if (mcrd_card_check(chan) == MCRD_SUCCESS) {
        out_cards[ret].port = port;
        out_cards[ret].card = card;
        ++ret;
      }
    }
  }

  return ret;
}

mcrd_result_t mcrd_card_open(const mcrd_id_t id) {
  mcrd_card_close();

  const int chan = (int)id.port * 16 + (int)id.card;

  mcrd_clear_events(MCRD_EV_SW);
  const mcrd_result_t status = mcrd_card_check(chan);
  if (status != MCRD_SUCCESS) return status;

  mcrd_clear_events(MCRD_EV_SW);
  _card_load(chan);
  const u32 ev = mcrd_wait_event(MCRD_EV_SW);
  printf("mcrd_card_open(%d, %d): event %08x\n", id.port, id.card, ev);

  switch (ev) {
    case EvSpTIMOUT: return MCRD_NO_CARD;
    case EvSpERROR:  return MCRD_ERROR;
    case EvSpNEW:    mcrd_chan = chan; return MCRD_UNFORMATTED;
    default:         mcrd_chan = chan; return MCRD_SUCCESS;
  }
}

mcrd_result_t mcrd_card_close(void) {
  if (mcrd_save_fd >= 0) {
    close(mcrd_save_fd);
    mcrd_save_fd = -1;
  }
  if (mcrd_chan >= 0) {
    memset(&mcrd_save_hdr, 0, sizeof(mcrd_save_hdr));
    mcrd_chan = -1;
  }
  return MCRD_SUCCESS;
}

mcrd_result_t mcrd_card_format(void) {
  if (mcrd_chan < 0)
    return MCRD_WRONG_CARD;
  if (format(mcrd_devname(mcrd_chan)))
    return MCRD_SUCCESS;
  return MCRD_ERROR;
}

u32 mcrd_save_slots_available(void) {
  if (mcrd_chan < 0 || mcrd_save_fd < 0)
    return 0;
  return mcrd_save_hdr.slotmask;
}

mcrd_result_t mcrd_save_open(const char *name) {
  if (mcrd_chan < 0)
    return MCRD_WRONG_CARD;

  char tmp[MCRD_SECSIZE];
  sprintf(tmp, "%s%s", mcrd_devname(mcrd_chan), name);

  int fd = open(tmp, 0x0001 | 0x0002); // O_RDWR
  if (fd < 0) {
    printf("mcrd_save_open(%s): could not find save at %s\n", name, tmp);
    return MCRD_NO_SAVE;
  }

  // read SAVEHDR
  int rx = read(fd, tmp, MCRD_SECSIZE);
  if (rx != MCRD_SECSIZE) {
    printf("mcrd_save_open(%s): tried to read %d bytes, got %d instead\n", name, MCRD_SECSIZE, rx);
    goto _error;
  }

  if (tmp[0] != 'S' || tmp[1] != 'C') {
    printf("mcrd_save_open(%s): invalid SAVEHDR\n", name);
    goto _error;
  }

  // seek past the icons
  lseek(fd, sizeof(SAVEHDR) + MCRD_SAVE_ICON_FRAMES * MCRD_SECSIZE, SEEK_SET);

  // read our save header
  rx = read(fd, (char *)&mcrd_save_hdr, sizeof(mcrd_save_hdr));
  if (rx != sizeof(mcrd_save_hdr)) {
    printf("mcrd_save_open(%s): tried to read %d bytes, got %d instead\n", name, sizeof(mcrd_save_hdr), rx);
    goto _error;
  }

  if (memcmp(mcrd_save_hdr.id, MCRD_SAVE_MAGIC, sizeof(mcrd_save_hdr.id)) != 0) {
    printf("mcrd_save_open(%s): invalid save block header\n", name, sizeof(mcrd_save_hdr), rx);
    goto _error;
  }

  mcrd_save_fd = fd;

  return MCRD_SUCCESS;

_error:
  mcrd_save_hdr.id[0] = 0;
  close(fd);
  return MCRD_ERROR;
}

mcrd_result_t mcrd_save_create(const char *name) {
  if (mcrd_chan < 0)
    return MCRD_WRONG_CARD;

  char tmp[MCRD_SECSIZE];
  sprintf(tmp, "%s%s", mcrd_devname(mcrd_chan), name);

  int fd = open(tmp, 0x0001 | 0x0002 | 0x0200 | (1 << 16)); // O_RDWR | O_CREAT | 1 block size
  if (fd < 0) {
    printf("mcrd_save_create(%s): could not create save at %s\n", name, tmp);
    return MCRD_NO_SPACE;
  }

  // write common memcard header
  SAVEHDR savehdr;
  savehdr.id[0] = 'S';
  savehdr.id[1] = 'C';
  savehdr.size = 1; // 8kb should be enough for everybody
  savehdr.type = 0x10 + MCRD_SAVE_ICON_FRAMES;
  strncpy((char *)savehdr.title, name, sizeof(savehdr.title) - 2);
  memcpy(savehdr.clut, save_icon_clut, sizeof(save_icon_clut));
  write(fd, (char *)&savehdr, sizeof(savehdr));

  // write save icons (assume they're one after the other)
  write(fd, (char *)save_icon_data, sizeof(save_icon_data));

  // write our save header
  memcpy(mcrd_save_hdr.id, MCRD_SAVE_MAGIC, sizeof(mcrd_save_hdr.id));
  mcrd_save_hdr.slotmask = 0; // new save file
  write(fd, (char *)&mcrd_save_hdr, sizeof(mcrd_save_hdr));

  mcrd_save_fd = fd;

  return MCRD_SUCCESS;
}

mcrd_result_t mcrd_save_write_slot(const int slot, const void *data, const int size) {
  if (mcrd_chan < 0)
    return MCRD_WRONG_CARD;

  if (mcrd_save_fd < 0) {
    printf("mcrd_save_write_slot(%d): writing with no save open\n", slot);
    return MCRD_NO_SAVE;
  }

  if (!data || !size || size > MCRD_MAX_SAVE_SIZE || slot >= MCRD_MAX_SAVES)
    return MCRD_ERROR;

  // update the slot mask

  mcrd_save_hdr.slotmask |= (1 << slot);

  int ofs = sizeof(SAVEHDR) + MCRD_SECSIZE * MCRD_SAVE_ICON_FRAMES;
  if (lseek(mcrd_save_fd, ofs, SEEK_SET) < 0) {
    printf("mcrd_save_write_slot(%d): error seeking to %d\n", slot, ofs);
    return MCRD_ERROR;
  }

  // unfortunately we cannot write just 4 bytes, so we gotta rewrite the entire save header
  write(mcrd_save_fd, (char *)&mcrd_save_hdr, sizeof(mcrd_save_hdr));

  // write the save itself

  ofs = sizeof(SAVEHDR) + MCRD_SECSIZE * MCRD_SAVE_ICON_FRAMES +
    sizeof(mcrd_save_hdr) + MCRD_MAX_SAVE_SIZE * slot;
  if (lseek(mcrd_save_fd, ofs, SEEK_SET) < 0) {
    printf("mcrd_save_write_slot(%d): error seeking to %d\n", slot, ofs);
    return MCRD_ERROR;
  }

  write(mcrd_save_fd, (char *)data, size);

  return MCRD_SUCCESS;
}

mcrd_result_t mcrd_save_read_slot(const int slot, void *data, const int size) {
  if (mcrd_chan < 0)
    return MCRD_WRONG_CARD;

  if (mcrd_save_fd < 0) {
    printf("mcrd_save_read_slot(%d): reading with no save open\n", slot);
    return MCRD_NO_SAVE;
  }

  if ((mcrd_save_hdr.slotmask & (1 << slot)) == 0) {
    printf("mcrd_save_read_slot(%d): no save in this slot\n", slot);
    return MCRD_NO_SAVE;
  }

  if (!data || !size || size > MCRD_MAX_SAVE_SIZE || slot >= MCRD_MAX_SAVES)
    return MCRD_ERROR;

  const int ofs = sizeof(SAVEHDR) + MCRD_SECSIZE * MCRD_SAVE_ICON_FRAMES +
    sizeof(mcrd_save_hdr) + MCRD_MAX_SAVE_SIZE * slot;
  if (lseek(mcrd_save_fd, ofs, SEEK_SET) < 0) {
    printf("mcrd_save_read_slot(%d): error seeking to %d\n", slot, ofs);
    return MCRD_ERROR;
  }

  read(mcrd_save_fd, data, size);

  return MCRD_SUCCESS;
}
