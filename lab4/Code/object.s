.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
  li $v0,4
  la $a0, _prompt
  syscall
  li $v0, 5
  syscall
  jr $ra
write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  jr $ra
main:
  addi $sp, $sp, -32
  sw $s0, ($sp)
  sw $s1, 4($sp)
  sw $s2, 8($sp)
  sw $s3, 12($sp)
  sw $s4, 16($sp)
  sw $s5, 20($sp)
  sw $s6, 24($sp)
  sw $s7, 28($sp)
  addi $sp, $sp, -44
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $s0, $v0
  move $s1, $s0
  li $t0, 0
  li $t1, 4
  mul $s2, $t0, $t1
  addi $t0, $sp, 0
  move $s3, $t0
  add $s4, $s3, $s2
  li $t0, 0
  move $s5, $t0
  li $t0, 1
  bgt $s1, $t0, label3
  j label4
label3:
  li $t0, 1
  move $s5, $t0
label4:
 sw $s5, ($s4)
  li $t0, 1
  li $t1, 4
  mul $s6, $t0, $t1
  addi $t0, $sp, 0
  move $s7, $t0
  lw $t1, 8($sp)
  add $t1, $s7, $s6
  sw $t1, 8($sp)
  lw $t0, 12($sp)
  li $t1, 0
  move $t0, $t1
  sw $t0, 12($sp)
  li $t0, 4
  blt $s1, $t0, label5
  j label6
label5:
  lw $t0, 12($sp)
  li $t1, 1
  move $t0, $t1
  sw $t0, 12($sp)
label6:
  lw $t0, 8($sp)
  lw $t1, 12($sp)
 sw $t1, ($t0)
  sw $t0, 8($sp)
  sw $t1, 12($sp)
  lw $t0, 16($sp)
  lw $t1, 8($sp)
  li $t2, 0
  li $t3, 0
  lw $t2, 0($s4)
  lw $t3, 0($t1)
  add $t0, $t2, $t3
  sw $t0, 16($sp)
  sw $t1, 8($sp)
  lw $t0, 16($sp)
  beq $t0, $s1, label1
  j label2
label1:
  li $t0, 996
  addi $sp, $sp, -4
  sw $a0, 0($sp)
  move $a0, $t0
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  lw $a0, 0($sp)
  addi $sp, $sp, 4
label2:
  lw $t0, 20($sp)
  li $t1, 0
  li $t2, 4
  mul $t0, $t1, $t2
  sw $t0, 20($sp)
  lw $t0, 24($sp)
  addi $t1, $sp, 0
  move $t0, $t1
  lw $t2, 28($sp)
  lw $t3, 20($sp)
  add $t2, $t0, $t3
  sw $t0, 24($sp)
  sw $t2, 28($sp)
  sw $t3, 20($sp)
  lw $t0, 28($sp)
  addi $sp, $sp, -4
  sw $a0, 0($sp)
  lw $a0, 0($t0)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  lw $a0, 0($sp)
  addi $sp, $sp, 4
  lw $t1, 32($sp)
  li $t2, 1
  li $t3, 4
  mul $t1, $t2, $t3
  sw $t0, 28($sp)
  sw $t1, 32($sp)
  lw $t0, 36($sp)
  addi $t1, $sp, 0
  move $t0, $t1
  lw $t2, 40($sp)
  lw $t3, 32($sp)
  add $t2, $t0, $t3
  sw $t0, 36($sp)
  sw $t2, 40($sp)
  sw $t3, 32($sp)
  lw $t0, 40($sp)
  addi $sp, $sp, -4
  sw $a0, 0($sp)
  lw $a0, 0($t0)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  lw $a0, 0($sp)
  addi $sp, $sp, 4
  addi $sp, $sp, -4
  sw $a0, 0($sp)
  move $a0, $s1
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  lw $a0, 0($sp)
  addi $sp, $sp, 4
  li $t1, 0
  move $v0, $t1
  sw $t0, 40($sp)
  addi $sp, $sp, 44
  lw $s7, 28($sp)
  lw $s6, 24($sp)
  lw $s5, 20($sp)
  lw $s4, 16($sp)
  lw $s3, 12($sp)
  lw $s2, 8($sp)
  lw $s1, 4($sp)
  lw $s0, ($sp)
  addi $sp, $sp, 32
  jr $ra
