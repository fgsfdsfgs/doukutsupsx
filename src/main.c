#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#include "engine/common.h"
#include "engine/timer.h"
#include "engine/memory.h"
#include "engine/sound.h"
#include "engine/org.h"
#include "engine/graphics.h"
#include "engine/filesystem.h"
#include "engine/input.h"
#include "game/game.h"

int main(int argc, char **argv) {
  gfx_init();
  cd_init();
  mem_init(); // have to do this AFTER CdInit for some reason
  snd_init(SFX_MAIN_BANK);
  org_init();
  in_init();

  // load main graphics bank
  gfx_load_gfx_bank(GFX_MAIN_BANK);
  gfx_init_fonts();

  // now we can init game-related stuff
  timer_init();
  game_init();

  // lo mark: all allocations after game_init() are unloaded every stagebank change
  // hi mark: dynamic allocations after stagebank load, if any
  mem_set_mark(MEM_MARK_LO);

  u32 now = timer_ticks;
  u32 next_frame = now;
  timer_next_orgtick = now;

  game_start();

  while (1) {
    now = timer_ticks;

    if (now >= next_frame) {
      in_update();
      game_frame();
      gfx_swap_buffers();
      next_frame = now + 2; 
    }

    const u32 orgwait = org_get_wait() / 10;
    if (now >= timer_next_orgtick && orgwait) {
      org_tick();
      timer_next_orgtick = now + orgwait;
    }
  }

  return 0;
}
