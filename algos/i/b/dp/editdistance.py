#	Edit distance; recursive and dynammic
#
#			 min(T(a, b, i + 1, j + 1), 1 + T(a, b, i + 1, j), 1 + T(a, b, i, j + 1))	if a[i] == b[j]
#	T(a, b, i, j) =	 min(1 + T(a, b, i + 1, j + 1), 1 + T(a, b, i + 1, j), 1 + T(a, b, i, j + 1))
#
#			# simplified
#			 min((a[i] != b[j]) + T(a, b, i + 1, j + 1), 1 + T(a, b, i + 1, j), 1 + T(a, b, i, j + 1))
#
#	The thought process here is that if two letters match, we can either use that matching and continue or replace
#	one of the letters with a "-". If the letters don't match, we can either count it as an edit or replace with a "-"
#
import sys

def edit_recursive(a, b, i, j):

	if i >= len(a):
		return len(b) - j

	if j >= len(b):
		return len(a) - i

	return (min((a[i] != b[j]) + edit_recursive(a, b, i + 1, j + 1), 1 + edit_recursive(a, b, i + 1, j),
	       1 + edit_recursive(a, b, i, j + 1)))

print(edit_recursive("a", "b", 0, 0))
print(edit_recursive("snowy", "sunny", 0, 0))
print(edit_recursive("exponential", "polynomial", 0, 0))


def edit_dynamic(a, b):
	MAX = sys.maxsize
	alen = len(a)
	blen = len(b)

	# use list comprehension to make a list of lists (or not)
	table = []
	for i in range(0, alen + 1):
		row = []
		for j in range(0, blen + 1):
			# caused an initial bug; make sure to think hard about the base
			# cases. the typical base cases of 0 or MAX don't work, it needs
			# to increment as we move down the board
			if i == 0:
				row.append(j)
			elif j == 0:
				row.append(i)
			else:
				row.append(0)

		table.append(row)


	for i in range(1, alen + 1):
		for j in range(1, blen + 1):
			table[i][j] = min((a[i - 1] != b[j - 1]) + table[i - 1][j - 1], 1 + table[i][j - 1], 1 + table[i - 1][j])

	return table[alen][blen]

print(edit_dynamic("a", "b"))
print(edit_dynamic("snowy", "sunny"))
print(edit_dynamic("exponential", "polynomial"))
