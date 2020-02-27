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
addiu $r2, $zero, 0x9
addiu $r3, $zero, 0x0
addiu $r6, $zero, 0x9
addiu $r5, $zero, 0x0
addiu $r3, $zero, 0x0
lw $r7, 0x0($r4) 
lw $r8, 0x4($r4) 
lw $r4, 0x4($r4) 
addiu $r10, $zero, 0x0
addiu $r11, $zero, 0x1
sub $r7, $r7, $r8
bgtz $r7, 0x0009 
addiu $r3, $r3, 0x1 
bne $r2, $r3, 0xFFF8 
lw $r4, $r1
addiu $r5, $r5, 0x1 
bne $r6, $r5, 0xFFF4 
bne $r10, $r11, 0x0005 
sw $r7, 0x0($r4)
sw $r8, 0x4($r4)
bne $r10, $r11, 0xFFF8 
