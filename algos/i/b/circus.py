# sort both, then find the longest common subsequence between the two
def circus(l):

	sorted_height = sorted(l, key = height)
	sorted_weight = sorted(l, key = weight)

	result = lcs(sorted_height, sorted_weight, 0, 0)
	return result

def height(p):
	(h, _) = p
	return h

def weight(p):
	(_, w) = p
	return w

def lcs(A, B, i, j):
	if i >= len(A):
		return 0
	if j >= len(B):
		return 0

	count = 0
	if A[i] == B[j]:
		count = 1

	return (max(count + lcs(A, B, i + 1, j + 1), lcs(A, B, i + 1, j),
		lcs(A, B, i, j + 1)))

assert(lcs([1, 2, 3, 4, 5], [1, 3, 4, 6, 7], 0, 0) == 3)

# just include backpointers
assert(circus([(65, 100), (70, 150), (56, 90), (75, 190), (60, 95), (68, 110)]) == 6)
assert(circus([(65, 100), (70, 150), (56, 90), (68, 190), (60, 95), (68, 110)]) == 5)

# convert to dp: TODO
