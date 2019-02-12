# can also convert this to a dp solution
def find_palindrome(l, i, j, count):
	if i == j:
		return count + 1
	if i == j - 1:
		return count + 2

	c = 0
	if l[i] == l[j]:
		c = 2

	return max(find_palindrome(l, i + 1, j - 1, c + count), find_palindrome(l, i, j - 1, 0),
		   find_palindrome(l, i + 1, j, 0))

assert(find_palindrome(["a"], 0, 0, 0) == 1)
assert(find_palindrome(["a", "b"], 0, 1, 0) == 2)
assert(find_palindrome(["a", "b", "b"], 0, 2, 0) == 2)
assert(find_palindrome(["a", "b", "a"], 0, 2, 0) == 3)
assert(find_palindrome(["d", "a", "b", "a", "e"], 0, 4, 0) == 3)
assert(find_palindrome(["d", "a", "b", "a", "d"], 0, 4, 0) == 5)
assert(find_palindrome(["a", "b", "b", "a", "d"], 0, 4, 0) == 4)
