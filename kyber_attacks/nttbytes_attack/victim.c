#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

extern void nttfrombytes_avx(__m256i *r, const uint8_t *a,
                             const __m256i *qdata);

#define ALIGNED_INT16(N)        \
  union {                       \
    int16_t coeffs[N];          \
    __m256i vec[(N + 15) / 16]; \
  }
typedef ALIGNED_INT16(256) poly;

typedef struct {
  poly vec[4];
} polyvec;

void poly_frombytes() {
  poly qdata, r;
  uint8_t a[384];

  for (int i = 0; i < 384; i++) {
    if (i % 32 == 0) printf("\n");
    a[i] = (i + (i >> 8)) % 256;
    // a[i] = (i + (i / 256)) % 256;

    printf("%02x ", a[i]);
  }

  while (1) {
    nttfrombytes_avx(r.vec, a, qdata.vec);
  }
}

void main() {
  poly_frombytes();
  return;
}