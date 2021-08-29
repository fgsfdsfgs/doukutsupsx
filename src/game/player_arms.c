#include <string.h>

#include "engine/common.h"
#include "engine/sound.h"
#include "engine/input.h"

#include "game/bullet.h"
#include "game/game.h"
#include "game/npc.h"
#include "game/player.h"
#include "game/caret.h"

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

    default:
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
          caret_spawn(player.x - (3 * 0x200), player.y - (10 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (3 * 0x200), player.y - (10 * 0x200), 1);
          caret_spawn(player.x + (3 * 0x200), player.y - (10 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else if (player.down) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (3 * 0x200), player.y + (10 * 0x200), 3);
          caret_spawn(player.x - (3 * 0x200), player.y + (10 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (3 * 0x200), player.y + (10 * 0x200), 3);
          caret_spawn(player.x + (3 * 0x200), player.y + (10 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (6 * 0x200), player.y + (2 * 0x200), 0);
          caret_spawn(player.x - (12 * 0x200), player.y + (2 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (6 * 0x200), player.y + (2 * 0x200), 2);
          caret_spawn(player.x + (12 * 0x200), player.y + (2 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      }

      snd_play_sound(PRIO_NORMAL, 33, FALSE);
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

    default:
      bul_no = 6;
      break;
  }

  if (bullet_count_by_arm(2) > 1) return;

  if (input_trig & IN_FIRE) {
    if (!plr_arm_use_ammo(1)) {
      snd_play_sound(PRIO_NORMAL, 37, FALSE);
    } else {
      if (player.up) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y - (8 * 0x200), 1);
          caret_spawn(player.x - (1 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y - (8 * 0x200), 1);
          caret_spawn(player.x + (1 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else if (player.down) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y + (8 * 0x200), 3);
          caret_spawn(player.x - (1 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y + (8 * 0x200), 3);
          caret_spawn(player.x + (1 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (6 * 0x200), player.y + (3 * 0x200), 0);
          caret_spawn(player.x - (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (6 * 0x200), player.y + (3 * 0x200), 2);
          caret_spawn(player.x + (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      }

      if (level == 3)
        snd_play_sound(PRIO_NORMAL, 49, FALSE);
      else
        snd_play_sound(PRIO_NORMAL, 32, FALSE);
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

    default:
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
          caret_spawn(player.x - (4 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (4 * 0x200), player.y - (8 * 0x200), 1);
          caret_spawn(player.x + (4 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else if (player.down) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (4 * 0x200), player.y + (8 * 0x200), 3);
          caret_spawn(player.x - (4 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (4 * 0x200), player.y + (8 * 0x200), 3);
          caret_spawn(player.x + (4 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (6 * 0x200), player.y + (2 * 0x200), 0);
          caret_spawn(player.x - (12 * 0x200), player.y + (2 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (6 * 0x200), player.y + (2 * 0x200), 2);
          caret_spawn(player.x + (12 * 0x200), player.y + (2 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      }

      snd_play_sound(PRIO_NORMAL, 34, FALSE);
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

    default:
      bul_no = 12;
      break;
  }

  if (!(input_held & IN_FIRE)) player.cooldown = 6;

  if (input_held & IN_FIRE) {
    if (++player.cooldown < 6) return;

    player.cooldown = 0;

    if (!plr_arm_use_ammo(1)) {
      snd_play_sound(PRIO_NORMAL, 37, FALSE);

      if (empty_cooldown == 0) {
        caret_spawn(player.x, player.y, CARET_EMPTY, DIR_LEFT);
        empty_cooldown = 50;
      }

      return;
    }

    if (player.up) {
      if (level == 3) player.yvel += 0x100;

      if (player.dir == 0) {
        bullet_spawn(bul_no, player.x - (3 * 0x200), player.y - (8 * 0x200), 1);
        caret_spawn(player.x - (3 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
      } else {
        bullet_spawn(bul_no, player.x + (3 * 0x200), player.y - (8 * 0x200), 1);
        caret_spawn(player.x + (3 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
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
        caret_spawn(player.x - (3 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
      } else {
        bullet_spawn(bul_no, player.x + (3 * 0x200), player.y + (8 * 0x200), 3);
        caret_spawn(player.x + (3 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
      }
    } else {
      if (player.dir == 0) {
        bullet_spawn(bul_no, player.x - (12 * 0x200), player.y + (3 * 0x200), 0);
        caret_spawn(player.x - (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
      } else {
        bullet_spawn(bul_no, player.x + (12 * 0x200), player.y + (3 * 0x200), 2);
        caret_spawn(player.x + (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
      }
    }

    if (level == 3)
      snd_play_sound(PRIO_NORMAL, 49, FALSE);
    else
      snd_play_sound(PRIO_NORMAL, 32, FALSE);
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
        if (bullet_count_by_arm(10) > 0) return;
        if (bullet_count_by_arm(11) > 0) return;
        bul_no = 28;
        break;

      case 2:
        if (bullet_count_by_arm(10) > 1) return;
        if (bullet_count_by_arm(11) > 1) return;
        bul_no = 29;
        break;

      default:
        if (bullet_count_by_arm(10) > 3) return;
        if (bullet_count_by_arm(11) > 3) return;
        bul_no = 30;
        break;
    }
  } else {
    switch (level) {
      case 1:
        if (bullet_count_by_arm(5) > 0) return;
        if (bullet_count_by_arm(6) > 0) return;
        bul_no = 13;
        break;

      case 2:
        if (bullet_count_by_arm(5) > 1) return;
        if (bullet_count_by_arm(6) > 1) return;
        bul_no = 14;
        break;

      default:
        if (bullet_count_by_arm(5) > 3) return;
        if (bullet_count_by_arm(6) > 3) return;
        bul_no = 15;
        break;
    }
  }

  if (input_trig & IN_FIRE) {
    if (level < 3) {
      if (!plr_arm_use_ammo(1)) {
        snd_play_sound(PRIO_NORMAL, 37, FALSE);

        if (empty_cooldown == 0) {
          caret_spawn(player.x, player.y, CARET_EMPTY, DIR_LEFT);
          empty_cooldown = 50;
        }

        return;
      }

      if (player.up) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y - (8 * 0x200), 1);
          caret_spawn(player.x - (1 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y - (8 * 0x200), 1);
          caret_spawn(player.x + (1 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else if (player.down) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y + (8 * 0x200), 3);
          caret_spawn(player.x - (1 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y + (8 * 0x200), 3);
          caret_spawn(player.x + (1 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (6 * 0x200), player.y, 0);
          caret_spawn(player.x - (12 * 0x200), player.y, CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (6 * 0x200), player.y, 2);
          caret_spawn(player.x + (12 * 0x200), player.y, CARET_SHOOT, DIR_LEFT);
        }
      }
    } else {
      if (!plr_arm_use_ammo(1)) {
        snd_play_sound(PRIO_NORMAL, 37, FALSE);

        if (empty_cooldown == 0) {
          caret_spawn(player.x, player.y, CARET_EMPTY, DIR_LEFT);
          empty_cooldown = 50;
        }

        return;
      }

      if (player.up) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y - (8 * 0x200), 1);
          caret_spawn(player.x - (1 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
          bullet_spawn(bul_no, player.x + (3 * 0x200), player.y, 1);
          bullet_spawn(bul_no, player.x - (3 * 0x200), player.y, 1);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y - (8 * 0x200), 1);
          caret_spawn(player.x + (1 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
          bullet_spawn(bul_no, player.x + (3 * 0x200), player.y, 1);
          bullet_spawn(bul_no, player.x - (3 * 0x200), player.y, 1);
        }
      } else if (player.down) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y + (8 * 0x200), 3);
          caret_spawn(player.x - (1 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
          bullet_spawn(bul_no, player.x + (3 * 0x200), player.y, 3);
          bullet_spawn(bul_no, player.x - (3 * 0x200), player.y, 3);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y + (8 * 0x200), 3);
          caret_spawn(player.x + (1 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
          bullet_spawn(bul_no, player.x - (3 * 0x200), player.y, 3);
          bullet_spawn(bul_no, player.x + (3 * 0x200), player.y, 3);
        }
      } else {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (6 * 0x200), player.y + (1 * 0x200), 0);
          caret_spawn(player.x - (12 * 0x200), player.y + (1 * 0x200), CARET_SHOOT, DIR_LEFT);
          bullet_spawn(bul_no, player.x, player.y - (8 * 0x200), 0);
          bullet_spawn(bul_no, player.x + (4 * 0x200), player.y - (1 * 0x200), 0);
        } else {
          bullet_spawn(bul_no, player.x + (6 * 0x200), player.y + (1 * 0x200), 2);
          caret_spawn(player.x + (12 * 0x200), player.y + (1 * 0x200), CARET_SHOOT, DIR_LEFT);
          bullet_spawn(bul_no, player.x, player.y - (8 * 0x200), 2);
          bullet_spawn(bul_no, player.x - (4 * 0x200), player.y - (1 * 0x200), 2);
        }
      }
    }

    snd_play_sound(PRIO_NORMAL, 32, FALSE);
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
      snd_play_sound(PRIO_NORMAL, 37, FALSE);

      if (empty_cooldown == 0) {
        caret_spawn(player.x, player.y, CARET_EMPTY, DIR_LEFT);
        empty_cooldown = 50;
      }

      return;
    }

    if (player.up) {
      if (player.dir == 0) {
        bullet_spawn(19, player.x - (1 * 0x200), player.y - (2 * 0x200), 1);
        caret_spawn(player.x - (1 * 0x200), player.y - (2 * 0x200), CARET_SHOOT, DIR_LEFT);
      } else {
        bullet_spawn(19, player.x + (1 * 0x200), player.y - (2 * 0x200), 1);
        caret_spawn(player.x + (1 * 0x200), player.y - (2 * 0x200), CARET_SHOOT, DIR_LEFT);
      }
    } else if (player.down) {
      if (player.dir == 0) {
        bullet_spawn(19, player.x - (1 * 0x200), player.y + (2 * 0x200), 3);
        caret_spawn(player.x - (1 * 0x200), player.y + (2 * 0x200), CARET_SHOOT, DIR_LEFT);
      } else {
        bullet_spawn(19, player.x + (1 * 0x200), player.y + (2 * 0x200), 3);
        caret_spawn(player.x + (1 * 0x200), player.y + (2 * 0x200), CARET_SHOOT, DIR_LEFT);
      }
    } else {
      if (player.dir == 0) {
        bullet_spawn(19, player.x - (6 * 0x200), player.y + (3 * 0x200), 0);
        caret_spawn(player.x - (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
      } else {
        bullet_spawn(19, player.x + (6 * 0x200), player.y + (3 * 0x200), 2);
        caret_spawn(player.x + (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
      }
    }

    snd_play_sound(PRIO_NORMAL, 48, FALSE);
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
      snd_play_sound(PRIO_NORMAL, 37, FALSE);

      if (empty_cooldown == 0) {
        caret_spawn(player.x, player.y, CARET_EMPTY, DIR_LEFT);
        empty_cooldown = 50;
      }

      return;
    }

    if (player.up) {
      if (player.dir == 0) {
        bullet_spawn(level, player.x - (3 * 0x200), player.y - (8 * 0x200), 1);
        caret_spawn(player.x - (3 * 0x200), player.y - (16 * 0x200), CARET_SHOOT, DIR_LEFT);
      } else {
        bullet_spawn(level, player.x + (3 * 0x200), player.y - (8 * 0x200), 1);
        caret_spawn(player.x + (3 * 0x200), player.y - (16 * 0x200), CARET_SHOOT, DIR_LEFT);
      }
    } else if (player.down) {
      if (player.dir == 0) {
        bullet_spawn(level, player.x - (3 * 0x200), player.y + (8 * 0x200), 3);
        caret_spawn(player.x - (3 * 0x200), player.y + (16 * 0x200), CARET_SHOOT, DIR_LEFT);
      } else {
        bullet_spawn(level, player.x + (3 * 0x200), player.y + (8 * 0x200), 3);
        caret_spawn(player.x + (3 * 0x200), player.y + (16 * 0x200), CARET_SHOOT, DIR_LEFT);
      }
    } else {
      if (player.dir == 0) {
        bullet_spawn(level, player.x - (6 * 0x200), player.y + (3 * 0x200), 0);
        caret_spawn(player.x - (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
      } else {
        bullet_spawn(level, player.x + (6 * 0x200), player.y + (3 * 0x200), 2);
        caret_spawn(player.x + (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
      }
    }

    snd_play_sound(PRIO_NORMAL, 48, FALSE);
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

    default:
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

    snd_play_sound(PRIO_NORMAL, 34, FALSE);
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

    default:
      bul_no = 36;
      break;
  }

  if (bullet_count_by_arm(12) > 1) return;

  if (input_trig & IN_FIRE) {
    if (!plr_arm_use_ammo(1)) {
      snd_play_sound(PRIO_NORMAL, 37, FALSE);
    } else {
      if (player.up) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y - (12 * 0x200), 1);
          caret_spawn(player.x - (1 * 0x200), player.y - (12 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y - (12 * 0x200), 1);
          caret_spawn(player.x + (1 * 0x200), player.y - (12 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else if (player.down) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y + (12 * 0x200), 3);
          caret_spawn(player.x - (1 * 0x200), player.y + (12 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y + (12 * 0x200), 3);
          caret_spawn(player.x + (1 * 0x200), player.y + (12 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (22 * 0x200), player.y + (3 * 0x200), 0);
          caret_spawn(player.x - (16 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (22 * 0x200), player.y + (3 * 0x200), 2);
          caret_spawn(player.x + (16 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      }

      switch (level) {
        case 1:
          snd_play_sound(PRIO_NORMAL, 117, FALSE);
          break;

        case 2:
          snd_play_sound(PRIO_NORMAL, 49, FALSE);
          break;

        case 3:
          snd_play_sound(PRIO_NORMAL, 60, FALSE);
          break;
      }
    }
  }
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
          snd_play_sound(PRIO_NORMAL, 59, FALSE);
          break;

        case 2:
          snd_play_sound(PRIO_NORMAL, 60, FALSE);
          break;

        case 3:
          if (!plr_arm_at_max_exp()) snd_play_sound(PRIO_NORMAL, 61, FALSE);

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
      snd_play_sound(PRIO_NORMAL, 65, FALSE);
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

    default:
      if (max)
        bul_no = 39;
      else
        bul_no = 38;
      break;
  }

  if (bullet_count_by_arm(13) > 0 || bullet_count_by_arm(14) > 0) return;

  if (input_trig & IN_FIRE || shot) {
    if (!plr_arm_use_ammo(1)) {
      snd_play_sound(PRIO_NORMAL, 37, FALSE);
    } else {
      if (player.up) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y - (8 * 0x200), 1);
          caret_spawn(player.x - (1 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y - (8 * 0x200), 1);
          caret_spawn(player.x + (1 * 0x200), player.y - (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else if (player.down) {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (1 * 0x200), player.y + (8 * 0x200), 3);
          caret_spawn(player.x - (1 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (1 * 0x200), player.y + (8 * 0x200), 3);
          caret_spawn(player.x + (1 * 0x200), player.y + (8 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      } else {
        if (player.dir == 0) {
          bullet_spawn(bul_no, player.x - (6 * 0x200), player.y + (3 * 0x200), 0);
          caret_spawn(player.x - (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
        } else {
          bullet_spawn(bul_no, player.x + (6 * 0x200), player.y + (3 * 0x200), 2);
          caret_spawn(player.x + (12 * 0x200), player.y + (3 * 0x200), CARET_SHOOT, DIR_LEFT);
        }
      }

      switch (bul_no) {
        case 6:
          snd_play_sound(PRIO_NORMAL, 49, FALSE);
          break;

        case 37:
          snd_play_sound(PRIO_NORMAL, 62, FALSE);
          break;

        case 38:
          snd_play_sound(PRIO_NORMAL, 63, FALSE);
          break;

        case 39:
          snd_play_sound(PRIO_NORMAL, 64, FALSE);
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

plr_arm_data_t *plr_arm_find(const u8 id) {
  for (int i = 0; i < player.num_arms; ++i) {
    if (player.arms[i].id == id)
      return &player.arms[i];
  }
  return NULL;
}

plr_arm_data_t *plr_arm_find_missile_launcher(void) {
  // the player can only own one of these at a time
  for (int i = 0; i < player.num_arms; ++i) {
    if (player.arms[i].id == 5 || player.arms[i].id == 10)
      return &player.arms[i];
  }
  return NULL;
}

void plr_arm_reset_exp(void) {
  player.arms[player.arm].level = 1;
  player.arms[player.arm].exp = 0;
}

void plr_arm_add_exp(int val) {
  plr_arm_data_t *arm = &player.arms[player.arm];

  int lv = arm->level - 1;

  arm->exp += val;

  if (lv == 2) {
    if (arm->exp >= plr_arms_exptab[arm->id][2]) {
      arm->exp = plr_arms_exptab[arm->id][2];
      if (player.equip & EQUIP_WHIMSICAL_STAR) {
        if (player.star < PLR_MAX_STAR)
          ++player.star;
      }
    }
  } else {
    for (; lv < 2; ++lv) {
      if (arm->exp >= plr_arms_exptab[arm->id][lv]) {
        ++arm->level;
        arm->exp = 0;
        if (arm->id != 13) {
          snd_play_sound(PRIO_NORMAL, 27, FALSE);
          caret_spawn(player.x, player.y, CARET_LEVEL_UP, DIR_LEFT);
        }
      }
    }
  }

  if (arm->id != 13) {
    player.exp_count += val;
    player.exp_wait = 30;
  } else {
    player.exp_wait = 10;
  }
}

bool plr_arm_at_max_exp(void) {
  if (player.arms[player.arm].level == 3) {
    if (player.arms[player.arm].exp >= plr_arms_exptab[player.arms[player.arm].id][2])
      return TRUE;
  }
  return FALSE;
}

void plr_arm_reset_spur_charge(void) {
  spur_charge = 0;
  if (player.arms[player.arm].id == 13)
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
  player.arm = 0;
  if (player.num_arms) {
    player.arms_x = 32;
    snd_play_sound(PRIO_NORMAL, 4, FALSE);
  }
}

int plr_arm_swap_to_next(void) {
  if (player.num_arms == 0)
    return 0;

  // original game always plays the change effects
  player.arms_x = 32;
  player.arm = (player.arm + 1) % player.num_arms;
  snd_play_sound(PRIO_NORMAL, 4, FALSE);

  return player.arm;
}

int plr_arm_swap_to_prev(void) {
  if (player.num_arms == 0)
    return 0;

  // original game always plays the change effects
  player.arms_x = 0;
  player.arm = (player.arm + player.num_arms - 1) % player.num_arms;
  snd_play_sound(PRIO_NORMAL, 4, FALSE);

  return player.arm;
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
  if (!(player.cond & PLRCOND_INVISIBLE))
    arm_functab[player.arms[player.arm].id](player.arms[player.arm].level);
}

bool plr_arm_give(const int id, const int max_ammo) {
  plr_arm_data_t *arm = plr_arm_find(id);
  if (arm == NULL) {
    // add new arm
    const int idx = player.num_arms++;
    arm = &player.arms[idx];
    memset_word(arm, 0, sizeof(*arm));
    arm->id = id;
    arm->level = 1;
  }

  arm->max_ammo += max_ammo;
  arm->ammo += max_ammo;
  if (arm->ammo > arm->max_ammo)
    arm->ammo = arm->max_ammo;

  return TRUE;
}

bool plr_arm_take(const int id) {
  plr_arm_data_t *arm = plr_arm_find(id);
  if (arm == NULL) return FALSE;
  // nuke it from the array
  const int idx = arm - player.arms + 1;
  const int count = player.num_arms - idx;
  if (count) memcpy_word(arm, &player.arms[idx], count * sizeof(*arm));
  --player.num_arms;
  // swap to first weapon in case it was selected
  plr_arm_swap_to_first();
  return TRUE;
}

bool plr_arm_trade(const int id, const int new_id, const int new_max_ammo) {
  plr_arm_data_t *arm = plr_arm_find(id);
  if (arm == NULL) return FALSE;

  // replace it with the new weapon in place
  arm->id = new_id;
  arm->level = 1;
  arm->exp = 0;
  arm->max_ammo += new_max_ammo;
  arm->ammo += new_max_ammo;

  return TRUE;
}

void plr_arms_refill_all(void) {
  for (int i = 0; i < player.num_arms; ++i)
    player.arms[i].ammo = player.arms[i].max_ammo;
}

void plr_arms_empty_all(void) {
  for (int i = 0; i < player.num_arms; ++i) {
    player.arms[i].ammo = 0;
    player.arms[i].exp = 0;
    player.arms[i].level = 1;
  }
}
