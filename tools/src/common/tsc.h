#pragma once

#include <stdint.h>

#define TSC_MAX_SIZE 0x5000 // original game limitation
#define TSC_MAX_EVENTS 1024 // max events per script

#pragma pack(push, 1)

typedef struct {
  uint16_t id;  // event number (the 4 digits after the '#')
  uint16_t ofs; // offset to event code from start of tsc_script_t
} tsc_event_t;

typedef struct {
  uint32_t num_ev;      // number of events in script
  tsc_event_t ev_map[]; // [num_ev]
  // compiled tsc bytecode follows
} tsc_script_t;

#pragma pack(pop)

// requires a NULL-terminated string
tsc_script_t *tsc_compile(char *src, const int in_size, int *out_size);

void tsc_decode(uint8_t *data, const int size);

// these scan in the source text since it's easier this way

// scan tsc for music changes
int tsc_scan_music(const char *src, uint32_t *songlist, const int limit);

// scan tsc for potential stage transitions
int tsc_scan_transitions(const char *src, uint32_t *linklist, const uint32_t ignore, const int limit);
