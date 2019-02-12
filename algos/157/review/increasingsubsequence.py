#!/usr/bin/python3

#                                             1             if i >= len(k)
#                                     T(k, i + 1, i + 2)    if j >= len(k)
#               T(k, i, j) = max {    T(k, j, j + 1}        if k[j] >= k[i]      }
#                                     T(k, i, j + 1)

def recursive(k, i, j):
        # base case; with only one element it'll be 1
        if i >= len(k):
                return 1
        if j >= len(k):
                return recursive(k, i + 1, i + 2)
        else:
                return max((k[j] >= k[i]) + recursive(k, j, j + 1), recursive(k, i, j + 1))

assert(recursive([5, 2, 8], 0, 1) == 2)
assert(recursive([6, 5, 8, 4], 0, 1) == 2)
assert(recursive([5, 2, 1], 0, 1) == 1)
assert(recursive([5, 2, 8, 6, 3, 6, 9, 7], 0, 1))  == 4

def dynamic(k):
	size = len(k)

	# https://stackoverflow.com/questions/6667201/how-to-define-a-two-dimensional-array-in-python
	table = [[0 for j in range(size + 1)] for i in range(size + 1)]

	# let's let i be the rows and j be in the columns -- always follow the recurrence. In this
	# case we'll fill the matrix up to down, left to right since we'll need to substitute j for i
	for j in range(1, size + 1):
		for i in range(j, size + 1):
			if k[i - 1] >= k[j - 1]:
				table[i][j] = max(1 + table[j][j - 1], table[i][j - 1])
			else:
				table[i][j] = max(table[j][j - 1], table[i][j - 1])

	return table[size][size]

assert(dynamic([5, 4, 3]) == 1)
assert(dynamic([5, 2, 8]) == 2)
assert(dynamic([6, 5, 8, 4]) == 2)
assert(dynamic([5, 2, 1]) == 1)
assert(dynamic([5, 2, 8, 6, 3, 6, 9, 7])  == 4)
assert(dynamic([1, 2, 3, 4, 5, 6, 7]) == 7)
