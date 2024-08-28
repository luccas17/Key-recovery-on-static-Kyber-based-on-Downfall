CC ?= /usr/bin/cc
CFLAGS += -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wredundant-decls \
  -Wshadow -Wpointer-arith -mavx2 -mbmi2 -mpopcnt -maes \
  -march=native -mtune=native -O3 -fomit-frame-pointer
NISTFLAGS += -Wno-unused-result -mavx2 -mbmi2 -mpopcnt -maes \
  -march=native -mtune=native -O3 -fomit-frame-pointer
RM = /bin/rm

SOURCES = kem.c indcpa.c polyvec.c poly.c fq.S shuffle.S ntt.S invntt.S \
  basemul.S consts.c rejsample.c cbd.c verify.c
SOURCESKECCAK   = $(SOURCES) fips202.c fips202x4.c symmetric-shake.c \
  keccak4x/KeccakP-1600-times4-SIMD256.o
SOURCESNINETIES = $(SOURCES) aes256ctr.c
HEADERS = params.h align.h kem.h indcpa.h polyvec.h poly.h reduce.h fq.inc shuffle.inc \
  ntt.h consts.h rejsample.h cbd.h verify.h symmetric.h randombytes.h
HEADERSKECCAK   = $(HEADERS) fips202.h fips202x4.h
HEADERSNINETIES = $(HEADERS) sha2.h aes256ctr.h

.PHONY: all shared clean

all: \
  kyber \
  test_possible_keys \
  
shared: \
  libpqcrystals_kyber512_avx2.so \
  libpqcrystals_kyber768_avx2.so \
  libpqcrystals_kyber1024_avx2.so \
  libpqcrystals_kyber512-90s_avx2.so \
  libpqcrystals_kyber768-90s_avx2.so \
  libpqcrystals_kyber1024-90s_avx2.so \
  libpqcrystals_fips202_ref.so \
  libpqcrystals_fips202x4_avx2.so \
  libpqcrystals_aes256ctr_avx2.so

keccak4x/KeccakP-1600-times4-SIMD256.o: \
  keccak4x/KeccakP-1600-times4-SIMD256.c \
  keccak4x/KeccakP-1600-times4-SnP.h \
  keccak4x/KeccakP-1600-unrolling.macros \
  keccak4x/KeccakP-SIMD256-config.h \
  keccak4x/KeccakP-align.h \
  keccak4x/KeccakP-brg_endian.h
	$(CC) $(CFLAGS) -c $< -o $@

libpqcrystals_fips202_ref.so: fips202.c fips202.h
	$(CC) -shared -fPIC $(CFLAGS) -o $@ $<

libpqcrystals_fips202x4_avx2.so: fips202x4.c fips202x4.h \
  keccak4x/KeccakP-1600-times4-SIMD256.c \
  keccak4x/KeccakP-1600-times4-SnP.h \
  keccak4x/KeccakP-1600-unrolling.macros \
  keccak4x/KeccakP-SIMD256-config.h \
  keccak4x/KeccakP-align.h \
  keccak4x/KeccakP-brg_endian.h
	$(CC) -shared -fPIC $(CFLAGS) -o $@ $< keccak4x/KeccakP-1600-times4-SIMD256.c

libpqcrystals_aes256ctr_avx2.so: aes256ctr.c aes256ctr.h
	$(CC) -shared -fPIC $(CFLAGS) -o $@ $<

libpqcrystals_kyber512_avx2.so: $(SOURCES) $(HEADERS) symmetric-shake.c
	$(CC) -shared -fpic $(CFLAGS) -DKYBER_K=2 $(SOURCES) \
	  symmetric-shake.c -o libpqcrystals_kyber512_avx2.so

libpqcrystals_kyber768_avx2.so: $(SOURCES) $(HEADERS) symmetric-shake.c
	$(CC) -shared -fpic $(CFLAGS) -DKYBER_K=3 $(SOURCES) \
	  symmetric-shake.c -o libpqcrystals_kyber768_avx2.so

libpqcrystals_kyber1024_avx2.so: $(SOURCES) $(HEADERS) symmetric-shake.c
	$(CC) -shared -fpic $(CFLAGS) -DKYBER_K=4 $(SOURCES) \
	  symmetric-shake.c -o libpqcrystals_kyber1024_avx2.so


kyber: $(SOURCESKECCAK) $(HEADERSKECCAK) victim.c randombytes.c
	$(CC) $(CFLAGS) $(SOURCESKECCAK) randombytes.c victim.c -o victim

test_possible_keys: $(SOURCESKECCAK) $(HEADERSKECCAK) test_possible_keys.c randombytes.c
	$(CC) $(CFLAGS) $(SOURCESKECCAK) randombytes.c test_possible_keys.c -o test_possible_keys

clean:
	-$(RM) -rf *.o *.a *.so
	-$(RM) -rf victim
	-$(RM) -rf keccak4x/KeccakP-1600-times4-SIMD256.o
