lui $r0, 0x1001
addiu $r1, $zero, 0x5
sw $r1, 0x4($r0)
addiu $r1, $zero, 0x3
sw $r1, 0x8($r0)
addiu $r1, $zero, 0x6
sw $r1, 0xC($r0)
addiu $r1, $zero, 0x8
sw $r1, 0x10($r0)

addiu $r2, $zero, 0x10
addiu $r3, $zero, 0x1

addiu $r3, $r3, 0x1
bne $r2, $r3, 0xFFFF


