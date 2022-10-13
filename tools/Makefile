CC ?= gcc
CXX ?= g++
CFLAGS := -std=gnu99 -g -Og -Wno-unused-result
CXXFLAGS := -std=gnu++11 -g -Og -Wno-unused-result
LDFLAGS := -static -lm
LIBPSXAV_SRC := $(wildcard src/libpsxav/*.c)

all: orgconv.exe sfxconv.exe surfpack.exe stagepack.exe creditspack.exe trigcalc.exe tscc.exe

orgconv.exe: src/orgconv.c $(LIBPSXAV_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

sfxconv.exe: src/sfxconv.c $(LIBPSXAV_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

surfpack.exe: src/surfpack.c src/common/vram.c src/common/surface.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

stagepack.exe: src/stagepack.c src/common/vram.c src/common/surface.c src/common/stage.c src/common/tsc.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

creditspack.exe: src/creditspack.c src/common/vram.c src/common/surface.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

trigcalc.exe: src/trigcalc.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

tscc.exe: src/tscc.c src/common/tsc.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

img2h.exe: src/img2h.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# this one will only compile under mingw
fontgen.exe: src/fontgen.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) -lmingw32 -lgdi32 -lddraw

clean:
	rm -f *.exe

.PHONY: clean
