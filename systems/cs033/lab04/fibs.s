.file "fibs.s"							# Linker stuff (you don't need to worry about this)
.text

##################
# fibs_recursive #
##################

.globl fibs_recursive						# Linker stuff
.type	fibs_recursive, @function

fibs_recursive:							# unsigned int fibs_recursive(unsigned int n) {
	movq %rdi, %rax						#
	cmp $0, %rax						#
	je L1							#
	cmp $3, %rax						#
	jl L2							#
	pushq %rdi						#
	subq $1, %rdi						#
	call fibs_recursive					#
	popq %rdi						#
	pushq %rax						#
	pushq %rdi						#
	subq $2, %rdi						#
	call fibs_recursive					#
	popq %rdi						#
	popq %r8						#
	addq %r8, %rax						#
	jmp L3							#
L1:								#
	movq $0, %rax						#
	jmp L3							#
L2:								#
	movq $1, %rax						#
	jmp L3							#
L3:								#
	ret							#
								# }

.size	fibs_recursive, .-fibs_recursive  # Linker stuff

.section	.note.GNU-stack,"",@progbits
