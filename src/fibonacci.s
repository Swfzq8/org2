addiu $r1, $r0, 0x1
addiu $r2, $r0, 0x1
addiu $r3, $r0, 0x4
addiu $r4, $r0, 0x0
addu $r1, $r1, $r2
addu $r2, $r2, $r1
addiu $r4, $r4, 0x1
bne $r4, $r3, 0xfffd
