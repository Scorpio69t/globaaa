# check if a word is a permutation of a palindrome; key with any palindrome
# problem is that the occurences of a particular character should be even
# except for one. O(n) | n = length of string.


def check(s):
	if len(s) <= 2:
		return True

	d = prep(s)
	flag = False
	for keys in d:
		if d[keys] % 2 != 0:
			if flag:
				return False
			else:
				flag = True

	return True

# let's place contents of string into a dictionary
def prep(s):
	d = {}
	s = s.replace(" ", "").lower()

	for c in s:
		if c in d:
			d[c] = d[c] + 1
		else:
			d[c] = 1

	return d

assert(check("a"))
assert(check("ab"))
assert(check("Tact Coa"))
assert(not check("Tact Cot"))
