#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#define BYTE_TRY 4
#define WORD_SIZE 8
#define LENGTH 2

extern void load_VPGATHERQQ(char *src, uint64_t *indexes);
extern void load_VPGATHERDD(char *src, uint32_t *indexes);
extern void load_VMOVDQA(char *src);
extern void load_VMOVDQU(char *src);
extern void load_VMOVUPS(char *src);
extern void load_VPMASKMOVQ(char *src);
extern void store_VCOMPRESSPD(char *src, char *dst);

uint32_t indexD[8] = {0, 4, 8, 12, 16, 20, 24, 28};
uint64_t indexQ[4] = {0, 8, 16, 24};
char charArray[BYTE_TRY * WORD_SIZE * 2];
char dstArray[BYTE_TRY * WORD_SIZE * 2];

void initiateCharArray()
{
  uint64_t start = 16 * 3; // Start at 0x30
  for (int i = 0; i < BYTE_TRY * WORD_SIZE * 2; i++)
  {
    charArray[i] = start + i;
  }
}

void printArrayAsChars(char *pr)
{
  for (int j = 0; j < LENGTH; j++)
  {
    for (int i = 0; i < BYTE_TRY * WORD_SIZE; i++)
    {
      printf("%02x ", pr[i + j * BYTE_TRY * WORD_SIZE]);
    }
    printf("\n");
  }
  printf("\n");
}

int main(int argc, char *argv[])
{
  initiateCharArray();
  printArrayAsChars(charArray);
  while (1)
  {
#if VICTIM_VPGATHERQQ
    load_VPGATHERQQ(charArray, indexQ);
#elif VICTIM_VPGATHERDD
    load_VPGATHERDD(charArray, indexD);
#elif VICTIM_VMOVDQA
    load_VMOVDQA(charArray);
#elif VICTIM_VMOVDQU
    load_VMOVDQU(charArray);
#elif VICTIM_VMOVUPS
    load_VMOVUPS(charArray);
#elif VICTIM_VPMASKMOVQ
    load_VPMASKMOVQ(charArray);
#elif VICTIM_VCOMPRESSPD
    store_VCOMPRESSPD(charArray, dstArray);
#else
    printf("Forgot to set victim option\n");
    return 0;
#endif
  }
  return 0;
}
