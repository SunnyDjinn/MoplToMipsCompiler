	.data
_CR_:	.asciiz	"\n"
fibonacci:	.word 0, 0
	.text
	j main			# Directly jump to label main -> where we usually enter the program
	nop
fibonacciSeq:
	move	$t4, $sp	# Temporarily store $sp into $t4
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 1
	sw	$fp, ($sp)	# Store $fp on the stack
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 2
	sw	$ra, ($sp)	# Store $ra on the stack
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 3
	sw	$t4, ($sp)	# Store $t4 on the stack
	move	$fp, $sp	# Set new value of $fp to $sp
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 4
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 5
	## If Statement
	lw	$t0, 20($sp)	# Load value of $k[0] into $t0
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 6
	sw	$t0, ($sp)	# Store $t0 on the stack
	li	$t0, 3		# Load immediate 3 into $t0
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 7
	sw	$t0, ($sp)	# Store $t0 on the stack
	# Operation
	lw	$t1, ($sp)	# Load top of the stack in $t1
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 6
	lw	$t0, ($sp)	# Load top of the stack in $t0
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 5
	li	$t2, -1
	blt	$t0, $t1, e1
	nop
	li	$t2, 0
e1:
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 6
	sw	$t2, ($sp)	# Store $t2 on the stack
	# End operation
	lw	$t0, ($sp)	# Load top of the stack in $t0
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 5
	beq	$t0, $0, endif0
	nop
	li	$t0, 1		# Load immediate 1 into $t0
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 6
	sw	$t0, ($sp)	# Store $t0 on the stack
	lw	$t0, ($sp)	# Load top of the stack in $t0
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 5
	sw	$t0, 24($sp)	# Store $t0 into return value allocated space in $sp
	lw	$fp, 16($sp)	# restore the previous value of fp from $sp
	lw	$ra, 12($sp)	# restore the previous value of ra from $sp
	lw	$sp, 8($sp)	# restore the previous value of sp from $sp
	jr $ra			# Jump back to calling function
	nop
endif0:
	## End If statement
	## Affectation
	li	$t0, 2		# Load immediate 2 into $t0
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 6
	sw	$t0, ($sp)	# Store $t0 on the stack
	lw	$t0, ($sp)	# Load top of the stack in $t0
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 5
	sw	$t0, 4($sp)	# Store value of $t0 into $i[0]
	## End affectation
	## Write call
	li	$t0, 1		# Load immediate 1 into $t0
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 6
	sw	$t0, ($sp)	# Store $t0 on the stack
	lw	$t0, ($sp)	# Load top of the stack in $t0
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 5
	move	$a0, $t0	# Move $t0 into $a0
	li	$v0, 1		# Load 1 into $v0 for write call
	syscall			# System call
	la	$a0, _CR_	# Move carriage return into $a0
	li	$v0, 4		# Load 4 into $v0 for write call
	syscall			# System call
	## End write call
	## Write call
	la	$t0, fibonacci	# Load address of fibonacci into $t0
	lw	$t0, 4($t0)	# Load value of fibonacci[1] into $t0
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 6
	sw	$t0, ($sp)	# Store $t0 on the stack
	lw	$t0, ($sp)	# Load top of the stack in $t0
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 5
	move	$a0, $t0	# Move $t0 into $a0
	li	$v0, 1		# Load 1 into $v0 for write call
	syscall			# System call
	la	$a0, _CR_	# Move carriage return into $a0
	li	$v0, 4		# Load 4 into $v0 for write call
	syscall			# System call
	## End write call
	## While Statement
while2:
	lw	$t0, 4($sp)	# Load value of $i[0] into $t0
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 6
	sw	$t0, ($sp)	# Store $t0 on the stack
	lw	$t0, 24($sp)	# Load value of $k[0] into $t0
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 7
	sw	$t0, ($sp)	# Store $t0 on the stack
	# Operation
	lw	$t1, ($sp)	# Load top of the stack in $t1
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 6
	lw	$t0, ($sp)	# Load top of the stack in $t0
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 5
	li	$t2, -1
	blt	$t0, $t1, e3
	nop
	li	$t2, 0
e3:
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 6
	sw	$t2, ($sp)	# Store $t2 on the stack
	# End operation
	lw	$t0, ($sp)	# Load top of the stack in $t0
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 5
	beq	$t0, $0, endWhile2
	nop
	## Affectation
	la	$t0, fibonacci	# Load address of fibonacci into $t0
	lw	$t0, 0($t0)	# Load value of fibonacci[0] into $t0
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 6
	sw	$t0, ($sp)	# Store $t0 on the stack
	la	$t0, fibonacci	# Load address of fibonacci into $t0
	lw	$t0, 4($t0)	# Load value of fibonacci[1] into $t0
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 7
	sw	$t0, ($sp)	# Store $t0 on the stack
	# Operation
	lw	$t1, ($sp)	# Load top of the stack in $t1
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 6
	lw	$t0, ($sp)	# Load top of the stack in $t0
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 5
	add	$t2, $t0, $t1	# Add $t0 and $t1 and store in $t2
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 6
	sw	$t2, ($sp)	# Store $t2 on the stack
	# End operation
	lw	$t0, ($sp)	# Load top of the stack in $t0
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 5
	sw	$t0, 0($sp)	# Store value of $t0 into $next[0]
	## End affectation
	## Affectation
	la	$t0, fibonacci	# Load address of fibonacci into $t0
	lw	$t0, 4($t0)	# Load value of fibonacci[1] into $t0
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 6
	sw	$t0, ($sp)	# Store $t0 on the stack
	lw	$t0, ($sp)	# Load top of the stack in $t0
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 5
	la	$t1, fibonacci	# Load address of fibonacci into $t1
	sw	$t0, 0($t1)	# Store $t0 into fibonacci[0]
	## End affectation
	## Affectation
	lw	$t0, 0($sp)	# Load value of $next[0] into $t0
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 6
	sw	$t0, ($sp)	# Store $t0 on the stack
	lw	$t0, ($sp)	# Load top of the stack in $t0
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 5
	la	$t1, fibonacci	# Load address of fibonacci into $t1
	sw	$t0, 4($t1)	# Store $t0 into fibonacci[1]
	## End affectation
	## Affectation
	lw	$t0, 4($sp)	# Load value of $i[0] into $t0
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 6
	sw	$t0, ($sp)	# Store $t0 on the stack
	li	$t0, 1		# Load immediate 1 into $t0
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 7
	sw	$t0, ($sp)	# Store $t0 on the stack
	# Operation
	lw	$t1, ($sp)	# Load top of the stack in $t1
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 6
	lw	$t0, ($sp)	# Load top of the stack in $t0
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 5
	add	$t2, $t0, $t1	# Add $t0 and $t1 and store in $t2
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 6
	sw	$t2, ($sp)	# Store $t2 on the stack
	# End operation
	lw	$t0, ($sp)	# Load top of the stack in $t0
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 5
	sw	$t0, 4($sp)	# Store value of $t0 into $i[0]
	## End affectation
	## Write call
	lw	$t0, 0($sp)	# Load value of $next[0] into $t0
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 6
	sw	$t0, ($sp)	# Store $t0 on the stack
	lw	$t0, ($sp)	# Load top of the stack in $t0
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 5
	move	$a0, $t0	# Move $t0 into $a0
	li	$v0, 1		# Load 1 into $v0 for write call
	syscall			# System call
	la	$a0, _CR_	# Move carriage return into $a0
	li	$v0, 4		# Load 4 into $v0 for write call
	syscall			# System call
	## End write call
	j while2
	nop
endWhile2:
	## End While Statement
	lw	$t0, 0($sp)	# Load value of $next[0] into $t0
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 6
	sw	$t0, ($sp)	# Store $t0 on the stack
	lw	$t0, ($sp)	# Load top of the stack in $t0
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 5
	sw	$t0, 24($sp)	# Store $t0 into return value allocated space in $sp
	lw	$fp, 16($sp)	# restore the previous value of fp from $sp
	lw	$ra, 12($sp)	# restore the previous value of ra from $sp
	lw	$sp, 8($sp)	# restore the previous value of sp from $sp
	jr $ra			# Jump back to calling function
	nop
main:
	move	$t4, $sp	# Temporarily store $sp into $t4
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 1
	sw	$fp, ($sp)	# Store $fp on the stack
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 2
	sw	$ra, ($sp)	# Store $ra on the stack
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 3
	sw	$t4, ($sp)	# Store $t4 on the stack
	move	$fp, $sp	# Set new value of $fp to $sp
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 4
	## Affectation
	li	$t0, 1		# Load immediate 1 into $t0
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 5
	sw	$t0, ($sp)	# Store $t0 on the stack
	lw	$t0, ($sp)	# Load top of the stack in $t0
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 4
	la	$t1, fibonacci	# Load address of fibonacci into $t1
	sw	$t0, 0($t1)	# Store $t0 into fibonacci[0]
	## End affectation
	## Affectation
	li	$t0, 1		# Load immediate 1 into $t0
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 5
	sw	$t0, ($sp)	# Store $t0 on the stack
	lw	$t0, ($sp)	# Load top of the stack in $t0
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 4
	la	$t1, fibonacci	# Load address of fibonacci into $t1
	sw	$t0, 4($t1)	# Store $t0 into fibonacci[1]
	## End affectation
	## Affectation
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 5
	# Read call
	li	$v0, 5		# Load 5 into $v0 for read call
	syscall			# System call for read call
	# End read call
	subu	$sp, $sp, 4	# Allocate a new word on the stack, nbAlloc = 6
	sw	$v0, ($sp)	# Store $v0 on the stack
	jal fibonacciSeq		# Jump to function fibonacciSeq and return address is next instruction
	nop
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 5
	lw	$t0, ($sp)	# Load top of the stack in $t0
	addu	$sp, $sp, 4	# Free a word from the stack, nbAlloc = 4
	sw	$t0, 0($sp)	# Store value of $t0 into $result[0]
	## End affectation
	### EXIT
	li	$v0, 10		# Load 10 into $v0 for exit call 
	syscall			# System call
