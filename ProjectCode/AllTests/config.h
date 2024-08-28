// Attacker choices
// Choice of gather
#define VPGATHERQQ 1
#define VPGATHERDD 0

// Choice of permutation (only 1 implemented for these tests)
#define PERMQ 1
// #define PERMD 0

// How many bytes needed for a hit
#define MIN_BYTES 8

// Victim choices
#define VICTIM_VPGATHERQQ 0
#define VICTIM_VPGATHERDD 0
#define VICTIM_VMOVDQA 0
#define VICTIM_VMOVDQU 0
#define VICTIM_VMOVUPS 0
#define VICTIM_VPMASKMOVQ 1 // Extra choice, not seen in project report
