	.file	"my_atoi.c"
	.text
.globl my_atoi
	.type	my_atoi, @function
my_atoi:
	pushq	%rbp		# a callee-saved register; push onto stack
	movq	%rsp, %rbp	# move stack pointer to point to base pointer
	pushq	%rbx		# another callee-saver register

#####################################################################
# DON'T EDIT ABOVE THIS LINE                                        ######################################################################
# atoi(char *buffer)                                                #
# parse a character string into an integer.                         #
# %rdi contains a pointer to the array of characters (string).      #
# Place your return value in %rax at the end.                       #
#                                                                   #
# Use registers %rax, %rcx, and %r8 - %r11.                         #
#####################################################################
# Write your code here...                                           #
#####################################################################

# C code for what I'm about to write in assembly:
#
# int atoi(char* buffer) {
#   int acc = 0;
#
#   for(int i = 0; buffer[i] != '\0'; i++) {
#     int c = buffer[i] - 48;
#
#     if(c >= 0 && c <= 9) {
#       acc = acc * 10;
#       acc = acc + c;
#     }
#     else
#	return acc;
#   }
#
#   return acc;
#

	movq $0, %r8 		# move 0 into register %r8 (acc)

L1:
	movq (%rdi), %rax	# move value of parameter into %rax
	cmp $0, %al 		# compare with '\0'; %al == one byte
	je L2
	sub $48, %al		# get ascii value
	cmp $0, %al		# make sure %al >= 0
	jl L2
	cmp $9, %al		# make sure %al <= 9
	jg L2
	imul $10, %r8		# multiple acc by 10
	mov %al, %r9b		# move value into %r9
	add %r9, %r8 		# add in character
	add $1, %rdi		# update pointer to string
	jmp L1

L2:
	movq %r8, %rax		# move into %rax to be returned
##############################
# DON'T EDIT BELOW THIS LINE #
##############################
	popq	%rbx
	leave
	ret

	.size	my_atoi, .-my_atoi
	.ident	"GCC: (Debian 4.4.5-8) 4.4.5"
	.section	.note.GNU-stack,"",@progbits
