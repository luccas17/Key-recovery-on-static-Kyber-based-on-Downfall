#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BYTE_TRY 4
#define WORD_SIZE 8
#define LENGTH 2

extern void load_array(char *src);

char charArray[BYTE_TRY * WORD_SIZE * 2];

void initiateCharArray() {
  uint64_t start = 16 * 3;  // Start at 0x30
  for (int i = 0; i < BYTE_TRY * WORD_SIZE * 2; i++) {
    charArray[i] = start + i;
  }
}

void printArrayAsChars(char *pr) {
  for (int j = 0; j < LENGTH; j++) {
    for (int i = 0; i < BYTE_TRY * WORD_SIZE; i++) {
      printf("%02x ", pr[i + j * BYTE_TRY * WORD_SIZE]);
    }
    printf("\n");
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  initiateCharArray();
  printArrayAsChars(charArray);
  while (1) {
    load_array(charArray);
  }
  return 0;
}
