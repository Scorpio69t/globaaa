# this screams dynamic programming, so first, let's try to do it
# via recursion:
#
#  T(i) = max(l[i] + T[i - 1], T[i - 1])
#
# This doesn't return the actual list, but it will return the max value. This solution also only works
# if there are both positive and negative numbers in the list.

# O(2^n)
def contiguous_seq(l, i, len, sum):
        if i >= len:
                return sum
        else:
                return max(contiguous_seq(l, i + 1, len, sum + l[i]), contiguous_seq(l, i + 1, len, 0), sum)

assert(contiguous_seq([], 0, 0, 0) == 0)
assert(contiguous_seq([2, -8, 3, -2, 4, -10], 0, 6, 0) == 5)
assert(contiguous_seq([2, -1, 3, -2, -4, 10], 0, 6, 0) == 10)
assert(contiguous_seq([2, -1, 3, -2, 7, -6], 0, 6, 0) == 9)

# trick above: since numbers have to be sequential, pass value as parameter. This behaves like tail recursion
# where we're doing our calculation first before the recursive call. This is arguably better since the call
# stack doesn't have to be kept around once the recursion is over. Using the above recurrence, let's rewrite this
# as a dynamic programming problem.

# O(2n) = O(n)
def dynamic_seq(l):
	table = []

	# base case of zero
	table.append(0)

	for i in range(1, len(l) + 1):
		# just like the recurrence above: add in previous value, set it to zero, or start
		# sum anew
		table.append(max(table[i - 1] + l[i - 1], 0, l[i - 1]))

	return max(table)

assert(dynamic_seq([]) == 0)
assert(dynamic_seq([2, -8, 3, -2, 4, -10]) == 5)
assert(dynamic_seq([2, -1, 3, -2, -4, 10]) == 10)
assert(dynamic_seq([2, -1, 3, -2, 7, -6]) == 9)
