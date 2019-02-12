#
# Solution 1: Given a number N, iteratively add 1 to it, check to see
# if it's a palindrome, return when True.
#

def check_palin(n):
	# convert to string
	s = str(n)

	# set up indices to iterate over both edges simultaneously
	i = 0
	j = len(s) - 1

	while i <= j:
		if s[i] != s[j]:
			return False
		i = i + 1
		j = j - 1

	return True

# Linear search
def find_palin_linear(n):
	while True:
		if check_palin(n):
			return n
		else:
			n = n + 1

# Start search at n + 1 to ensure found palindrome > n
def linear_wrapper(n):
	return find_palin_linear(n + 1)

#
# Solution 2: Taking the structure of a palindrome into account,
# recursively change characters from the left and right handling
# corner cases correctly.
#
# In the function signature below, s is a list representation of the
# string, i and j are indices from the left and right, respectively.
# Returns a tuple: (whether value was increased, updated s).
#

def find_palin_recursive(s, i, j):
	# base case
	if i > j:
		return False, s

	# try to increment value at s[i]. If s[i] now equals 0
	# value has looped i.e. not increased
	if i == j:
		if s[i] == '9':
			s[i] = '0'
			return False, s
		else:
			s[i] = str(int(s[i]) + 1)
			return True, s

	else:
		s[j] = s[i]
		increased, s = find_palin_recursive(s, i + 1, j - 1)
		if not increased:
			if s[i] == '9':
				s[i] = '0'
				s[j] = s[i]
				return increased, s
			else:
				s[i] = str(int(s[i]) + 1)
				s[j] = s[i]
				return True, s
		else:
			return increased, s


def recursive_wrapper(n):
	# pass in number converted to list of strings along with indices
	increased, s = find_palin_recursive(list(str(n)), 0, len(str(n)) - 1)

	# if number never increased, change its length
	if not increased:
		s = ['1'] + s
		s[-1] = '1'
		return ''.join(s)
	else:
		return ''.join(s)

# run
count = input()
for i in range(0, count):
	n = input()
	print(linear_wrapper(n))
