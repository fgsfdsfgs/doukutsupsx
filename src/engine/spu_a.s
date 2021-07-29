.set noreorder

.include "hwregs_a.h"

.section .text

# this is a copy of the function from psn00b that fixes a bug
# where the function tests for 0xffff instead of 0x7ffff
.global spu_set_transfer_addr
.type spu_set_transfer_addr, @function
spu_set_transfer_addr:
  li    $v0, 0x1000              # Check if value is valid
  blt   $a0, $v0, .Lbad_value
  nop
  li    $v0, 0x7ffff
  bgt   $a0, $v0, .Lbad_value
  nop
  
  la    $v1, _spu_transfer_addr
  srl   $v0, $a0, 3              # Set transfer destination address
  sh    $v0, 0($v1)
  
  jr    $ra
  move  $v0, $a0
  
.Lbad_value:
  jr    $ra
  move  $v0, $0
