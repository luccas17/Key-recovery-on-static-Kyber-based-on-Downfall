.data 
.text 
  .global load_array 
load_array: 
  # rdi: charArray 
  VMOVDQU (%rdi), %ymm4 
  ret 