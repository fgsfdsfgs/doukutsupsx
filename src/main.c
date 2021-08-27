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
#include "engine/mcrd.h"
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

  // all allocations after game_init() are unloaded every stagebank change
  // high mark is changed during credits
  mem_set_mark(MEM_MARK_LO);
  mem_set_mark(MEM_MARK_HI);

  u32 now = timer_ticks;
  u32 next_frame = now;

  game_start_intro();

  while (1) {
    now = timer_ticks;
    if (now >= next_frame) {
      in_update();
      game_frame();
      gfx_swap_buffers();
      next_frame = now + 2; 
    }
  }

  return 0;
}
