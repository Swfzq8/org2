lui $r4, 0x1001
addiu $r0, $zero, 0x5
sw $r0, 0x0($r4)
addiu $r0, $zero, 0x3
sw $r0, 0x4($r4)
addiu $r0, $zero, 0x6
sw $r0, 0x8($r4)
addiu $r0, $zero, 0x8
sw $r0, 0x12($r4)
addiu $r0, $zero, 0x9
sw $r0, 0x16($r4)
addiu $r0, $zero, 0x1
sw $r0, 0x20($r4)
addiu $r0, $zero, 0x4
sw $r0, 0x24($r4)
addiu $r0, $zero, 0x7
sw $r0, 0x26($r4)
addiu $r0, $zero, 0x2
sw $r0, 0x28($r4)
addiu $r0, $zero, 0xA
sw $r0, 0x32($r4)
addiu $r2, $zero, 0xA
addiu $r3, $zero, 0x0
addiu $r6, $zero, 0xA
addiu $r5, $zero, 0x0
addiu $r3, $zero, 0x0
lw $r7, 0x0($r4) 
lw $r8, 0x4($r4) 
lw $r4, 0x4($r4) 
addiu $r10, $zero, 0x0
addiu $r11, $zero, 0x1
bge $r7, $r8, 0x0009 
addiu $r3, $r3, 0x1 
bne $r2, $r3, 0xFFF9 
lw $r4, 0xFFD8($r4)
addiu $r5, $r5, 0x1 
bne $r6, $r5, 0xFFF7 
bne $r10, $r11, 0x0005 
sw $r7, 0x0($r4)
sw $r8, 0x4($r4)
bne $r10, $r11, 0xFFF8 
