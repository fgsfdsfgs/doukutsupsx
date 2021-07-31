#include <string.h>

#include "engine/common.h"
#include "engine/sound.h"
#include "engine/input.h"

#include "game/bullet.h"
#include "game/game.h"
#include "game/npc.h"
#include "game/player.h"

const s16 plr_arms_exptab[PLR_MAX_ARMS][3] = {
  { 0,  0,  100 },
  { 30, 40, 16  },
  { 10, 20, 10  },
  { 10, 20, 20  },
  { 30, 40, 10  },
  { 10, 20, 10  },
  { 10, 20, 30  },
  { 10, 20, 5   },
  { 10, 20, 100 },
  { 30, 60, 0   },
  { 30, 60, 10  },
  { 10, 20, 100 },
  { 1,  1,  1   },
  { 40, 60, 200 }
};

// snake, machinegun, nemesis and spur are first, since they can be traded for;
// weapons #6, #8 and #11 do not exist
static const s8 arms_order[] = {
  1, 4, 12, 13, 2, 3, 5, 7, 9, 10,
};

// reverse lookup (arm id -> arm order)
static const s8 arms_order_reverse[PLR_MAX_ARMS] = {
  -1, 0, 4, 5, 1, 6, -1, 7, -1, 8, 9, -1, 2, 3, 
};

static const int arms_order_num = sizeof(arms_order) / sizeof(*arms_order);

// using statics for these counters is not good, but that's how the original does it
// if I change this, this will probably fuck up some delicate timings or something
static int empty_cooldown;
static int spur_charge;

static void arm_act_null(int level) {
  // nada
}

static void arm_act_snake(int level) {
  int bul_no;

  switch (level) {
    case 1:
      bul_no = 1;
      break;

    case 2:
      bul_no = 2;
      break;

    case 3:
      bul_no = 3;
      break;
  }

  if (bullet_count_by_arm(1) > 3) return;

  if (input_trig & IN_FIRE) {
    if (!plr_arm_use_ammo(1)) {
      plr_arm_swap_to_first();
    } else {
      if (player.up) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (3 * 0x200), player.y - (10 * 0x200), 1);
          // SetCaret(player.x - (3 * 0x200), player.y - (10 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (3 * 0x200), player.y - (10 * 0x200), 1);
          // SetCaret(player.x + (3 * 0x200), player.y - (10 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else if (player.down) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (3 * 0x200), player.y + (10 * 0x200), 3);
          // SetCaret(player.x - (3 * 0x200), player.y + (10 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (3 * 0x200), player.y + (10 * 0x200), 3);
          // SetCaret(player.x + (3 * 0x200), player.y + (10 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (6 * 0x200), player.y + (2 * 0x200), 0);
          // SetCaret(player.x - (12 * 0x200), player.y + (2 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (6 * 0x200), player.y + (2 * 0x200), 2);
          // SetCaret(player.x + (12 * 0x200), player.y + (2 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      }

      snd_play_sound(CHAN_ARMS, 33, SOUND_MODE_PLAY);
    }
  }
}

static void arm_act_polarstar(int level) {
  int bul_no;

  switch (level) {
    case 1:
      bul_no = 4;
      break;

    case 2:
      bul_no = 5;
      break;

    case 3:
      bul_no = 6;
      break;
  }

  if (bullet_count_by_arm(2) > 1) return;

  if (input_trig & IN_FIRE) {
    if (!plr_arm_use_ammo(1)) {
      snd_play_sound(CHAN_ARMS, 37, SOUND_MODE_PLAY);
    } else {
      if (player.up) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y - (8 * 0x200), 1);
          // SetCaret(player.x - (1 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y - (8 * 0x200), 1);
          // SetCaret(player.x + (1 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else if (player.down) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y + (8 * 0x200), 3);
          // SetCaret(player.x - (1 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y + (8 * 0x200), 3);
          // SetCaret(player.x + (1 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (6 * 0x200), player.y + (3 * 0x200), 0);
          // SetCaret(player.x - (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (6 * 0x200), player.y + (3 * 0x200), 2);
          // SetCaret(player.x + (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      }

      if (level == 3)
        snd_play_sound(CHAN_ARMS, 49, SOUND_MODE_PLAY);
      else
        snd_play_sound(CHAN_ARMS, 32, SOUND_MODE_PLAY);
    }
  }
}

static void arm_act_fireball(int level) {
  int bul_no;

  switch (level) {
    case 1:
      if (bullet_count_by_arm(3) > 1) return;

      bul_no = 7;
      break;

    case 2:
      if (bullet_count_by_arm(3) > 2) return;

      bul_no = 8;
      break;

    case 3:
      if (bullet_count_by_arm(3) > 3) return;

      bul_no = 9;
      break;
  }

  if (input_trig & IN_FIRE) {
    if (!plr_arm_use_ammo(1)) {
      plr_arm_swap_to_first();
    } else {
      if (player.up) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (4 * 0x200), player.y - (8 * 0x200), 1);
          // SetCaret(player.x - (4 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (4 * 0x200), player.y - (8 * 0x200), 1);
          // SetCaret(player.x + (4 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else if (player.down) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (4 * 0x200), player.y + (8 * 0x200), 3);
          // SetCaret(player.x - (4 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (4 * 0x200), player.y + (8 * 0x200), 3);
          // SetCaret(player.x + (4 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (6 * 0x200), player.y + (2 * 0x200), 0);
          // SetCaret(player.x - (12 * 0x200), player.y + (2 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (6 * 0x200), player.y + (2 * 0x200), 2);
          // SetCaret(player.x + (12 * 0x200), player.y + (2 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      }

      snd_play_sound(CHAN_ARMS, 34, SOUND_MODE_PLAY);
    }
  }
}

static void arm_act_machinegun(int level) {
  int bul_no;
  static int wait;

  if (bullet_count_by_arm(4) > 4) return;

  switch (level) {
    case 1:
      bul_no = 10;
      break;

    case 2:
      bul_no = 11;
      break;

    case 3:
      bul_no = 12;
      break;
  }

  if (!(input_held & IN_FIRE)) player.cooldown = 6;

  if (input_held & IN_FIRE) {
    if (++player.cooldown < 6) return;

    player.cooldown = 0;

    if (!plr_arm_use_ammo(1)) {
      snd_play_sound(CHAN_ARMS, 37, SOUND_MODE_PLAY);

      if (empty_cooldown == 0) {
        // SetCaret(player.x, player.y, CARET_EMPTY, DIR_LEFT);
        empty_cooldown = 50;
      }

      return;
    }

    if (player.up) {
      if (level == 3) player.yvel += 0x100;

      if (player.dir == 0) {
        bullet_spawn(bul_no, player.x - (3 * 0x200), player.y - (8 * 0x200), 1);
        // SetCaret(player.x - (3 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
      } else {
        bullet_spawn(bul_no, player.x + (3 * 0x200), player.y - (8 * 0x200), 1);
        // SetCaret(player.x + (3 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
      }
    } else if (player.down) {
      if (level == 3) {
        if (player.yvel > 0) player.yvel /= 2;

        if (player.yvel > -0x400) {
          player.yvel -= 0x200;
          if (player.yvel < -0x400) player.yvel = -0x400;
        }
      }

      if (player.dir == 0) {
        bullet_spawn(bul_no, player.x - (3 * 0x200), player.y + (8 * 0x200), 3);
        // SetCaret(player.x - (3 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
      } else {
        bullet_spawn(bul_no, player.x + (3 * 0x200), player.y + (8 * 0x200), 3);
        // SetCaret(player.x + (3 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
      }
    } else {
      if (player.dir == 0) {
        bullet_spawn(bul_no, player.x - (12 * 0x200), player.y + (3 * 0x200), 0);
        // SetCaret(player.x - (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
      } else {
        bullet_spawn(bul_no, player.x + (12 * 0x200), player.y + (3 * 0x200), 2);
        // SetCaret(player.x + (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
      }
    }

    if (level == 3)
      snd_play_sound(CHAN_ARMS, 49, SOUND_MODE_PLAY);
    else
      snd_play_sound(CHAN_ARMS, 32, SOUND_MODE_PLAY);
  } else {
    ++wait;

    if (player.equip & EQUIP_TURBOCHARGE) {
      if (wait > 1) {
        wait = 0;
        plr_arm_charge_ammo(1);
      }
    } else {
      if (wait > 4) {
        wait = 0;
        plr_arm_charge_ammo(1);
      }
    }
  }
}

static inline void arm_act_missile_generic(int level, bool is_super) {
  int bul_no;

  if (is_super) {
    switch (level) {
      case 1:
        bul_no = 28;
        break;

      case 2:
        bul_no = 29;
        break;

      case 3:
        bul_no = 30;
        break;
    }

    switch (level) {
      case 1:
        if (bullet_count_by_arm(10) > 0) return;

        if (bullet_count_by_arm(11) > 0) return;

        break;

      case 2:
        if (bullet_count_by_arm(10) > 1) return;

        if (bullet_count_by_arm(11) > 1) return;

        break;

      case 3:
        if (bullet_count_by_arm(10) > 3) return;

        if (bullet_count_by_arm(11) > 3) return;

        break;
    }

  } else {
    switch (level) {
      case 1:
        bul_no = 13;
        break;

      case 2:
        bul_no = 14;
        break;

      case 3:
        bul_no = 15;
        break;
    }

    switch (level) {
      case 1:
        if (bullet_count_by_arm(5) > 0) return;

        if (bullet_count_by_arm(6) > 0) return;

        break;

      case 2:
        if (bullet_count_by_arm(5) > 1) return;

        if (bullet_count_by_arm(6) > 1) return;

        break;

      case 3:
        if (bullet_count_by_arm(5) > 3) return;

        if (bullet_count_by_arm(6) > 3) return;

        break;
    }
  }

  if (input_trig & IN_FIRE) {
    if (level < 3) {
      if (!plr_arm_use_ammo(1)) {
        snd_play_sound(CHAN_ARMS, 37, SOUND_MODE_PLAY);

        if (empty_cooldown == 0) {
          // SetCaret(player.x, player.y, CARET_EMPTY, DIR_LEFT);
          empty_cooldown = 50;
        }

        return;
      }

      if (player.up) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y - (8 * 0x200), 1);
          // SetCaret(player.x - (1 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y - (8 * 0x200), 1);
          // SetCaret(player.x + (1 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else if (player.down) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y + (8 * 0x200), 3);
          // SetCaret(player.x - (1 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y + (8 * 0x200), 3);
          // SetCaret(player.x + (1 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (6 * 0x200), player.y, 0);
          // SetCaret(player.x - (12 * 0x200), player.y, CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (6 * 0x200), player.y, 2);
          // SetCaret(player.x + (12 * 0x200), player.y, CARET_SHOOT, DIR_LEFT);
        }
      }
    } else {
      if (!plr_arm_use_ammo(1)) {
        snd_play_sound(CHAN_ARMS, 37, SOUND_MODE_PLAY);

        if (empty_cooldown == 0) {
          // SetCaret(player.x, player.y, CARET_EMPTY, DIR_LEFT);
          empty_cooldown = 50;
        }

        return;
      }

      if (player.up) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y - (8 * 0x200), 1);
          // SetCaret(player.x - (1 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
          bullet_spawn(bul_no, player.x + (3 * 0x200), player.y, 1);
          bullet_spawn(bul_no, player.x - (3 * 0x200), player.y, 1);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y - (8 * 0x200), 1);
          // SetCaret(player.x + (1 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
          bullet_spawn(bul_no, player.x + (3 * 0x200), player.y, 1);
          bullet_spawn(bul_no, player.x - (3 * 0x200), player.y, 1);
        }
      } else if (player.down) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y + (8 * 0x200), 3);
          // SetCaret(player.x - (1 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
          bullet_spawn(bul_no, player.x + (3 * 0x200), player.y, 3);
          bullet_spawn(bul_no, player.x - (3 * 0x200), player.y, 3);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y + (8 * 0x200), 3);
          // SetCaret(player.x + (1 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
          bullet_spawn(bul_no, player.x - (3 * 0x200), player.y, 3);
          bullet_spawn(bul_no, player.x + (3 * 0x200), player.y, 3);
        }
      } else {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (6 * 0x200), player.y + (1 * 0x200), 0);
          // SetCaret(player.x - (12 * 0x200), player.y + (1 * 0x200), CARET_SHOOT, DIR_LEFT);
          bullet_spawn(bul_no, player.x, player.y - (8 * 0x200), 0);
          bullet_spawn(bul_no, player.x + (4 * 0x200), player.y - (1 * 0x200), 0);
        } else {
          bullet_spawn(bul_no, player.x + (6 * 0x200), player.y + (1 * 0x200), 2);
          // SetCaret(player.x + (12 * 0x200), player.y + (1 * 0x200), CARET_SHOOT, DIR_LEFT);
          bullet_spawn(bul_no, player.x, player.y - (8 * 0x200), 2);
          bullet_spawn(bul_no, player.x - (4 * 0x200), player.y - (1 * 0x200), 2);
        }
      }
    }

    snd_play_sound(CHAN_ARMS, 32, SOUND_MODE_PLAY);
  }
}

static void arm_act_missile(int level) {
  arm_act_missile_generic(level, FALSE);
}

static void arm_act_supermissile(int level) {
  arm_act_missile_generic(level, TRUE);
}

static void arm_act_bubbler1(void) {
  static int wait;

  if (bullet_count_by_arm(7) > 3) return;

  if (input_trig & IN_FIRE) {
    if (!plr_arm_use_ammo(1)) {
      snd_play_sound(CHAN_ARMS, 37, SOUND_MODE_PLAY);

      if (empty_cooldown == 0) {
        // SetCaret(player.x, player.y, CARET_EMPTY, DIR_LEFT);
        empty_cooldown = 50;
      }

      return;
    }

    if (player.up) {
      if (player.dir == 0) {
        bullet_spawn(19, player.x - (1 * 0x200), player.y - (2 * 0x200), 1);
        // SetCaret(player.x - (1 * 0x200), player.y - (2 * 0x200), CARET_SHOOT, DIR_LEFT);
      } else {
        bullet_spawn(19, player.x + (1 * 0x200), player.y - (2 * 0x200), 1);
        // SetCaret(player.x + (1 * 0x200), player.y - (2 * 0x200), CARET_SHOOT, DIR_LEFT);
      }
    } else if (player.down) {
      if (player.dir == 0) {
        bullet_spawn(19, player.x - (1 * 0x200), player.y + (2 * 0x200), 3);
        // SetCaret(player.x - (1 * 0x200), player.y + (2 * 0x200), CARET_SHOOT, DIR_LEFT);
      } else {
        bullet_spawn(19, player.x + (1 * 0x200), player.y + (2 * 0x200), 3);
        // SetCaret(player.x + (1 * 0x200), player.y + (2 * 0x200), CARET_SHOOT, DIR_LEFT);
      }
    } else {
      if (player.dir == 0) {
        bullet_spawn(19, player.x - (6 * 0x200), player.y + (3 * 0x200), 0);
        // SetCaret(player.x - (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
      } else {
        bullet_spawn(19, player.x + (6 * 0x200), player.y + (3 * 0x200), 2);
        // SetCaret(player.x + (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
      }
    }

    snd_play_sound(CHAN_ARMS, 48, SOUND_MODE_PLAY);
  } else if (++wait > 20) {
    wait = 0;
    plr_arm_charge_ammo(1);
  }
}

static void arm_act_bubbler2(int level) {
  static int wait;

  if (bullet_count_by_arm(7) > 15) return;

  level += 18;

  if (!(input_held & IN_FIRE)) player.cooldown = 6;

  if (input_held & IN_FIRE) {
    if (++player.cooldown < 7) return;

    player.cooldown = 0;

    if (!plr_arm_use_ammo(1)) {
      snd_play_sound(CHAN_ARMS, 37, SOUND_MODE_PLAY);

      if (empty_cooldown == 0) {
        // SetCaret(player.x, player.y, CARET_EMPTY, DIR_LEFT);
        empty_cooldown = 50;
      }

      return;
    }

    if (player.up) {
      if (player.dir == 0) {
        bullet_spawn(level, player.x - (3 * 0x200), player.y - (8 * 0x200), 1);
        // SetCaret(player.x - (3 * 0x200), player.y - (16 * 0x200), CARET_SHOOT, DIR_LEFT);
      } else {
        bullet_spawn(level, player.x + (3 * 0x200), player.y - (8 * 0x200), 1);
        // SetCaret(player.x + (3 * 0x200), player.y - (16 * 0x200), CARET_SHOOT, DIR_LEFT);
      }
    } else if (player.down) {
      if (player.dir == 0) {
        bullet_spawn(level, player.x - (3 * 0x200), player.y + (8 * 0x200), 3);
        // SetCaret(player.x - (3 * 0x200), player.y + (16 * 0x200), CARET_SHOOT, DIR_LEFT);
      } else {
        bullet_spawn(level, player.x + (3 * 0x200), player.y + (8 * 0x200), 3);
        // SetCaret(player.x + (3 * 0x200), player.y + (16 * 0x200), CARET_SHOOT, DIR_LEFT);
      }
    } else {
      if (player.dir == 0) {
        bullet_spawn(level, player.x - (6 * 0x200), player.y + (3 * 0x200), 0);
        // SetCaret(player.x - (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
      } else {
        bullet_spawn(level, player.x + (6 * 0x200), player.y + (3 * 0x200), 2);
        // SetCaret(player.x + (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
      }
    }

    snd_play_sound(CHAN_ARMS, 48, SOUND_MODE_PLAY);
  } else if (++wait > 1) {
    wait = 0;
    plr_arm_charge_ammo(1);
  }
}

static void arm_act_sword(int level) {
  int bul_no;

  if (bullet_count_by_arm(9) > 0) return;

  switch (level) {
    case 1:
      bul_no = 25;
      break;

    case 2:
      bul_no = 26;
      break;

    case 3:
      bul_no = 27;
      break;
  }

  if (input_trig & IN_FIRE) {
    if (player.up) {
      if (player.dir == 0)
        bullet_spawn(bul_no, player.x - (1 * 0x200), player.y + (4 * 0x200), 1);
      else
        bullet_spawn(bul_no, player.x + (1 * 0x200), player.y + (4 * 0x200), 1);
    } else if (player.down) {
      if (player.dir == 0)
        bullet_spawn(bul_no, player.x - (1 * 0x200), player.y - (6 * 0x200), 3);
      else
        bullet_spawn(bul_no, player.x + (1 * 0x200), player.y - (6 * 0x200), 3);
    } else {
      if (player.dir == 0)
        bullet_spawn(bul_no, player.x + (6 * 0x200), player.y - (3 * 0x200), 0);
      else
        bullet_spawn(bul_no, player.x - (6 * 0x200), player.y - (3 * 0x200), 2);
    }

    snd_play_sound(CHAN_ARMS, 34, SOUND_MODE_PLAY);
  }
}

static void arm_act_bubbler(int level) {
  switch (level) {
    case 1:
      arm_act_bubbler1();
      break;
    default:
      arm_act_bubbler2(level);
      break;
  }
}

static void arm_act_nemesis(int level) {
  int bul_no;

  switch (level) {
    case 1:
      bul_no = 34;
      break;

    case 2:
      bul_no = 35;
      break;

    case 3:
      bul_no = 36;
      break;
  }

  if (bullet_count_by_arm(12) > 1) return;

  if (input_trig & IN_FIRE) {
    if (!plr_arm_use_ammo(1)) {
      snd_play_sound(CHAN_ARMS, 37, SOUND_MODE_PLAY);
    } else {
      if (player.up) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y - (12 * 0x200), 1);
          // SetCaret(player.x - (1 * 0x200), player.y - (12 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y - (12 * 0x200), 1);
          // SetCaret(player.x + (1 * 0x200), player.y - (12 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else if (player.down) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y + (12 * 0x200), 3);
          // SetCaret(player.x - (1 * 0x200), player.y + (12 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y + (12 * 0x200), 3);
          // SetCaret(player.x + (1 * 0x200), player.y + (12 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (22 * 0x200), player.y + (3 * 0x200), 0);
          // SetCaret(player.x - (16 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (22 * 0x200), player.y + (3 * 0x200), 2);
          // SetCaret(player.x + (16 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      }

      switch (level) {
        case 1:
          snd_play_sound(CHAN_ARMS, 117, SOUND_MODE_PLAY);
          break;

        case 2:
          snd_play_sound(CHAN_ARMS, 49, SOUND_MODE_PLAY);
          break;

        case 3:
          snd_play_sound(CHAN_ARMS, 60, SOUND_MODE_PLAY);
          break;
      }
    }
  }
}

void ResetspurCharge(void) {
  spur_charge = 0;
  if (player.arm == 13)
    plr_arm_reset_exp();
}

static void arm_act_spur(int level) {
  static bool max;
  int bul_no;
  bool shot = FALSE;

  if (input_held & IN_FIRE) {
    if (player.equip & EQUIP_TURBOCHARGE)
      plr_arm_add_exp(3);
    else
      plr_arm_add_exp(2);

    if (++spur_charge / 2 % 2) {
      switch (level) {
        case 1:
          snd_play_sound(CHAN_ARMS, 59, SOUND_MODE_PLAY);
          break;

        case 2:
          snd_play_sound(CHAN_ARMS, 60, SOUND_MODE_PLAY);
          break;

        case 3:
          if (!plr_arm_at_max_exp()) snd_play_sound(CHAN_ARMS, 61, SOUND_MODE_PLAY);

          break;
      }
    }
  } else {
    if (spur_charge)
      shot = TRUE;
    spur_charge = 0;
  }

  if (plr_arm_at_max_exp()) {
    if (!max) {
      max = TRUE;
      snd_play_sound(CHAN_ARMS, 65, SOUND_MODE_PLAY);
    }
  } else {
    max = FALSE;
  }

  if (!(input_held & IN_FIRE)) plr_arm_reset_exp();

  switch (level) {
    case 1:
      bul_no = 6;
      shot = FALSE;
      break;

    case 2:
      bul_no = 37;
      break;

    case 3:
      if (max)
        bul_no = 39;
      else
        bul_no = 38;

      break;
  }

  if (bullet_count_by_arm(13) > 0 || bullet_count_by_arm(14) > 0) return;

  if (input_trig & IN_FIRE || shot) {
    if (!plr_arm_use_ammo(1)) {
      snd_play_sound(CHAN_ARMS, 37, SOUND_MODE_PLAY);
    } else {
      if (player.up) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y - (8 * 0x200), 1);
          // SetCaret(player.x - (1 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y - (8 * 0x200), 1);
          // SetCaret(player.x + (1 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else if (player.down) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y + (8 * 0x200), 3);
          // SetCaret(player.x - (1 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y + (8 * 0x200), 3);
          // SetCaret(player.x + (1 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (6 * 0x200), player.y + (3 * 0x200), 0);
          // SetCaret(player.x - (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (6 * 0x200), player.y + (3 * 0x200), 2);
          // SetCaret(player.x + (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      }

      switch (bul_no) {
        case 6:
          snd_play_sound(CHAN_ARMS, 49, SOUND_MODE_PLAY);
          break;

        case 37:
          snd_play_sound(CHAN_ARMS, 62, SOUND_MODE_PLAY);
          break;

        case 38:
          snd_play_sound(CHAN_ARMS, 63, SOUND_MODE_PLAY);
          break;

        case 39:
          snd_play_sound(CHAN_ARMS, 64, SOUND_MODE_PLAY);
          break;
      }
    }
  }
}

typedef void (*arm_func_t)(int);
static const arm_func_t arm_functab[] = {
  arm_act_null,
  arm_act_snake,
  arm_act_polarstar,
  arm_act_fireball,
  arm_act_machinegun,
  arm_act_missile,
  arm_act_null,
  arm_act_bubbler,
  arm_act_null,
  arm_act_sword,
  arm_act_supermissile,
  arm_act_null,
  arm_act_nemesis,
  arm_act_spur,
};

void plr_arm_reset_exp(void) {
  player.arms[player.arm].level = 1;
  player.arms[player.arm].exp = 0;
}

void plr_arm_add_exp(int val) {
  int lv = player.arms[player.arm].level - 1;

  player.arms[player.arm].exp += val;

  if (lv == 2) {
    if (player.arms[player.arm].exp >= plr_arms_exptab[player.arm][2])
      player.arms[player.arm].exp = plr_arms_exptab[player.arm][2];
    if (player.equip & EQUIP_WHIMSICAL_STAR) {
      if (player.star < 3)
        ++ player.star;
    }
  } else {
    for (; lv < 2; ++lv) {
      if (player.arms[player.arm].exp >= plr_arms_exptab[player.arm][lv]) {
        ++player.arms[player.arm].level;
        player.arms[player.arm].exp = 0;
        if (player.arm != 13) {
          snd_play_sound(CHAN_MISC, 27, SOUND_MODE_PLAY);
          // SetCaret(player.x, player.y, CARET_LEVEL_UP, DIR_LEFT);
        }
      }
    }
  }

  if (player.arm != 13) {
    player.exp_count += val;
    player.exp_wait = 30;
  } else {
    player.exp_wait = 10;
  }
}

bool plr_arm_at_max_exp(void) {
  if (player.arms[player.arm].level == 3) {
    if (player.arms[player.arm].exp >= plr_arms_exptab[player.arm][2])
      return TRUE;
  }
  return FALSE;
}

void plr_arm_reset_spur_charge(void) {
  spur_charge = 0;
  if (player.arm == 13)
    plr_arm_reset_exp();
}

bool plr_arm_charge_ammo(const int val) {
  player.arms[player.arm].ammo += val;
  if (player.arms[player.arm].ammo > player.arms[player.arm].max_ammo)
    player.arms[player.arm].ammo = player.arms[player.arm].max_ammo;
  return TRUE;
}

bool plr_arm_use_ammo(const int val) {
  if (!player.arms[player.arm].max_ammo)
    return TRUE; // infinite ammo
  if (!player.arms[player.arm].ammo)
    return FALSE;

  player.arms[player.arm].ammo -= val;
  if (player.arms[player.arm].ammo < 0)
    player.arms[player.arm].ammo = 0;

  return TRUE;
}

void plr_arm_swap_to_first(void) {
  // swap to first available arm in the order
  for (int i = 0; i < arms_order_num; ++i) {
    if (player.arms[arms_order[i]].owned) {
      player.arm = arms_order[i];
      // gArmsEnergyX = 32;
      snd_play_sound(CHAN_MISC, 4, SOUND_MODE_PLAY);
      return;
    }
  }
  // got no weapons, just swap to empty
  player.arm = 0;
}

int plr_arm_swap_to_next(void) {
  const int start = arms_order_reverse[player.arm];
  if (start < 0) return 0;

  // original game always plays the change effects
  // gArmsEnergyX = 32;
  snd_play_sound(CHAN_MISC, 4, SOUND_MODE_PLAY);

  for (int i = 1; i < arms_order_num; ++i) {
    const int arm = arms_order[(start + i) % arms_order_num];
    if (player.arms[arm].owned) {
      player.arm = arm;
      return arm;
    }
  }

  return 0;
}

int plr_arm_swap_to_prev(void) {
  const int start = arms_order_reverse[player.arm];
  if (start < 0) return 0;

  // original game always plays the change effects
  // gArmsEnergyX = 32;
  snd_play_sound(CHAN_MISC, 4, SOUND_MODE_PLAY);

  for (int i = 1; i < arms_order_num - 1; ++i) {
    const int arm = arms_order[(start + arms_order_num - i) % arms_order_num];
    if (player.arms[arm].owned) {
      player.arm = arm;
      // gArmsEnergyX = 0;
      snd_play_sound(CHAN_MISC, 4, SOUND_MODE_PLAY);
      return arm;
    }
  }

  return 0;
}

void plr_arm_shoot(void) {
  static int soft_cooldown;

  if (empty_cooldown != 0)
    --empty_cooldown;

  // Only let the player shoot every 4 frames
  if (soft_cooldown != 0)
    --soft_cooldown;

  if (input_trig & IN_FIRE) {
    if (soft_cooldown != 0)
      return;
    soft_cooldown = 4;
  }

  // Run functions
  if (!(player.cond & 2))
    arm_functab[player.arm](player.arms[player.arm].level);
}

bool plr_arm_give(const int id, const int max_ammo) {
  if (player.arms[id].owned == FALSE) {
    memset(&player.arms[id], 0, sizeof(*player.arms));
    player.arms[id].level = 1;
    player.arms[id].owned = TRUE;
  }

  player.arms[id].max_ammo += max_ammo;
  player.arms[id].ammo += max_ammo;
  if (player.arms[id].ammo > player.arms[id].max_ammo)
    player.arms[id].ammo = player.arms[id].max_ammo;

  return TRUE;
}

bool plr_arm_take(const int id) {
  if (!player.arms[id].owned)
    return FALSE;
  player.arms[id].owned = FALSE;
  plr_arm_swap_to_first();
  return TRUE;
}
