.data 
.text 

  .global load_VPGATHERQQ
load_VPGATHERQQ: 
  # rdi: charArray
  # rsi: indexes
  vmovups (%rsi), %ymm1         # Indexes to gather
  vpcmpeqb %ymm2, %ymm2, %ymm2  # Bunch of ones 
  vpgatherqq %ymm2, 0(%rdi, %ymm1, 1), %ymm3 
  ret 

  .global load_VPGATHERDD
load_VPGATHERDD: 
  # rdi: charArray
  # rsi: indexes
  vmovups (%rsi), %ymm1         # Indexes to gather
  vpcmpeqb %ymm2, %ymm2, %ymm2  # Bunch of ones 
  vpgatherdd %ymm2, 0(%rdi, %ymm1, 1), %ymm3 
  ret 

  .global load_VMOVDQA
load_VMOVDQA: 
  # rdi: charArray 
  VMOVDQA (%rdi), %ymm4 
  ret 
  .global load_VMOVDQU 
load_VMOVDQU: 
  # rdi: charArray 
  VMOVDQU (%rdi), %ymm4 
  ret 

  .global load_VMOVUPS
load_VMOVUPS: 
  # rdi: charArray 
  VMOVUPS (%rdi), %ymm4 
  ret 

  .global load_VPMASKMOVQ
load_VPMASKMOVQ: 
  # rdi: charArray 
  vpxor %ymm2, %ymm2, %ymm2   # all zeros, default
  # vpcmpeqb %ymm2, %ymm2, %ymm2   # all ones 

  VPMASKMOVQ (%rdi), %ymm2, %ymm4 
  ret 

  .global store_VCOMPRESSPD
store_VCOMPRESSPD:
  VMOVUPS (%rdi), %ymm4 
  mov $15, %ebx 
  kmovb %ebx, %k1

x: 
  VCOMPRESSPD %ymm4, (%rsi) {%k1}
  jmp x

  ret 

