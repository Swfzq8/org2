
//lui r4, 0x1001;
//addiu $a0, $zero, 0x5
//sw $a0, 0($r4)
//addiu $a0, $zero,  0x3
//sw $a0, 4($r4)
//addiu $a0, $zero, 0x6
//sw $a0, 8($r4)
//addiu $a0, $zero, 0x8
//sw $a0, 12($r4)
//addiu $a0, $zero,  0x9
//sw $a0, 16($r4)
//addiu $a0, $zero, 0x1
//sw $a0, 20($r4)
//addiu $a0, $zero, 0x4
//sw $a0, 24($r4)
//addiu $a0, $zero,  0x7
//sw $a0, 26($r4)
//addiu $a0, $zero, 0x2
//sw $a0, 28($r4)
//addiu $a0, $zero, 0xA
//sw $a0, 32($r4)

lui $r0, 0x1001
addiu $r1, $zero, 0x5
sw $r1, 0x4($r0)
addiu $r1, $zero, 0x3
sw $r1, 0x8($r0)
addiu $r1, $zero, 0x6
sw $r1, 0xC($r0)
addiu $r1, $zero, 0x8
sw $r1, 0x10($r0)

slt r

addiu $r2, $zero, 0xA //value at 10 inner loop
addiu $r3, $zero, 0x0 //starting counter inner loop
addiu $r6, $zero, 0xA // value at 10 outer loop
addiu $r5, $zer0, 0x0 // starting counter outer loop

li $r3, 0x0 //resets the inner loop counter (happens after the 10 runs of inner loop and branched to by outer loop
lw $a0, 0($r4) //loads  value a[i]
lw $a1, 4($r4) // loads  value a[i+1]
sll $r4, 2 // shift in memory by 4 to increment to next element 
bge $a0, $a1,0x0009 //to update if a[i+1] is less than a[i]
addiu $r3, $r3, 0x1 // adding one to inner loop counter 
bne $r2, $r3, 0xFFFA //first for loop branches to line 42 
 //shift logical right 40 to go back to original address
addiu $r5, $r5, 0x1 //increment second for loop by one 
bne $r6, $r5, 0xFFF7 // increment to reset inner loop counter 

 // Update:
sw $a1, 0($r4)
sw $a0, 4(4r4)
b 0xFFF8 // branches to line 47 after swap values
