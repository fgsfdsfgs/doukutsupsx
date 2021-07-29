CC ?= gcc
LIBPSXAV_SRC := $(wildcard src/libpsxav/*.c)

all: orgconv.exe sfxconv.exe surfpack.exe stagepack.exe trigcalc.exe

orgconv.exe: src/orgconv.c $(LIBPSXAV_SRC)
	$(CC) -std=gnu99 -g -Og -o $@ $^ -static

sfxconv.exe: src/sfxconv.c $(LIBPSXAV_SRC)
	$(CC) -std=gnu99 -g -Og -o $@ $^ -static

surfpack.exe: src/surfpack.c src/common/vram.c src/common/surface.c
	$(CC) -std=gnu99 -g -Og -o $@ $^ -static

stagepack.exe: src/stagepack.c src/common/vram.c src/common/surface.c src/common/stage.c
	$(CC) -std=gnu99 -g -Og -o $@ $^ -static

trigcalc.exe: src/trigcalc.c
	$(CC) -std=gnu99 -g -Og -o $@ $^ -static

clean:
	rm -f *.exe

.PHONY: clean
