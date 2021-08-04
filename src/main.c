#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#include "engine/common.h"
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
  timer_start();
  game_init();

  // from now on, allocations persist until the end of the stage
  mem_set_mark();

  u32 now = timer_ticks;
  u32 next_frame = now;
  u32 next_orgtick = now;

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
    if (now >= next_orgtick && orgwait) {
      org_tick();
      next_orgtick = now + orgwait;
    }
  }

  return 0;
}
