lui $r4, 0x1001
addiu $r0, $zero, 0x5
sw $r0, 0x0($r4)
addiu $r0, $zero, 0x3
sw $r0, 0x4($r4)
addiu $r0, $zero, 0x6
sw $r0, 0x8($r4)
addiu $r0, $zero, 0x8
sw $r0, 0xC($r4)
addiu $r0, $zero, 0x9
sw $r0, 0x10($r4)
addiu $r0, $zero, 0x1
sw $r0, 0x14($r4)
addiu $r0, $zero, 0x4
sw $r0, 0x18($r4)
addiu $r0, $zero, 0x7
sw $r0, 0x1C($r4)
addiu $r0, $zero, 0x2
sw $r0, 0x20($r4)
addiu $r0, $zero, 0xA
sw $r0, 0x24($r4)
lw $r1, $r4
addiu $r15, $zero, 0x9
addiu $r17, $zero, 0x0
lw $r7, 0x0($r4) 
lw $r8, 0x4($r4) 
lw $r1, 0x4($r4)
addiu $r17, $r17, 0x1
sub $r3, $r7, $r1
bgtz $r3 0xfffb
sw $r1, 0x0($r4)
sw $r7 ,0x4($r4)
bne $r15, $r17, 0xfff8 
 
