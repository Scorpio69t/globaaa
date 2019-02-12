#
# Play the game mastermind; this is too hard to do in one pass. It's hard, since if you mark something
# with a "O", how do you know if it comes up again? If you have nested for loops, index i for original
# might map with some j for guess, so you want to mark it with a O, but you don't know if for some incremented
# i you'll actually get a "X" -- this is why you know you can't use nested for loops. This is why you should
# do it in multiple loops.
#

def hint(original, guess):
	# convert to lists
	original_list = []
	for c in original:
		original_list.append(c)

	guess_list = []
	for c in guess:
		guess_list.append(c)

	# results for hint
	result = []

	i = 0
	while(True):
		if len(original_list) == 0 or i >= len(original_list):
			break;

		# find all the 'X's first
		if guess_list[i] == original_list[i]:
			result.append("X")
			guess_list = guess_list[0:i] + guess_list[i+1:]
			original_list = original_list[0:i] + original_list[i+1:]

		i = i + 1

	i = 0
	while(True):
		if i >= len(original_list):
			break
		j = 0
		while(True):
			if j >= len(guess_list):
				break

			if guess_list[j] == original_list[i]:
				result.append("O")
				guess_list = guess_list[0:j] + guess_list[j+1:]
				original_list = original_list[0:i] + original_list[i+1:]
				break

			j = j + 1

		i = i + 1

	return ''.join(result)


assert(hint("abaa", "acdb") == "XO")
assert(hint("abab", "acab") == "XXO")
assert(hint("abce", "dcae") == "XOO")
