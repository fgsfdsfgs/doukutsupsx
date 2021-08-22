TARGET		= doukutsu
TYPE			= ps-exe

CFLAGS		:= -Isrc -Inugget/psyq/include
CFLAGS		+= -Wall -Wno-missing-braces -mno-check-zero-division

LDFLAGS		:= -Lnugget/psyq/lib -Wl,--start-group -lcard -lapi -lc2 -lcd -letc -lgpu -lspu -Wl,--end-group
LDFLAGS		+= -flto

SRCDIR		= src
SRCSUB		= engine game game/npc_act game/boss_act

SRCDIRS		= $(foreach dir,$(SRCSUB),$(SRCDIR)/$(dir)) $(SRCDIR)
CFILES		= $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))
CPPFILES 	= $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.cpp))
AFILES		= $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.s))
SRCS			= $(CFILES) $(CPPFILES) $(AFILES)
SRCS			+= nugget/common/crt0/crt0.s

default: all

iso: $(TARGET).iso

$(TARGET).iso: all
	mkpsxiso -y -q iso.xml

.PHONY: iso

include nugget/common.mk

CPPFLAGS_Release	+= -O3 -flto
LDFLAGS_Release		+= -O3 -flto
