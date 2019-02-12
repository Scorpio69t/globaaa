# Asana interview question: you have a rabbit in a carrot patch. Start the rabbit in the
# center on the square patch with the most carrots (if there's no exact center) and have
# him eat his way up, down, left, and right till he finds himself in a patch with 0 carrots.
def rabbit(garden, rowPos, colPos):
	# we've moved to a square with 0 carrots, which means this
	# was our best option. We should go to sleep.
	if garden[rowPos][colPos] == 0:
		return 0

	# carrots eaten in the current square
	yumyum = garden[rowPos][colPos]

	# mark the current square as visited
	garden[rowPos][colPos] = 0

	nextRow = None
	nextCol = None

	# let's now find where to go next; make sure to check square is
	# in the garden patch and it's not a square we've already eaten from
	if rowPos - 1 >= 0:
		nextRow = rowPos - 1
		nextCol = colPos

	if rowPos + 1 < len(garden):
		if nextRow == None or nextCol == None:
			nextRow = rowPos + 1
			nextCol = colPos
		# current square has more carrots than previously found future square
		elif garden[rowPos + 1][colPos] > garden[nextRow][nextCol]:
			nextRow = rowPos + 1
			nextCol = colPos

	if colPos - 1 >= 0:
		if nextRow == None or nextCol == None:
			nextRow = rowPos
			nextCol = colPos - 1

		elif garden[rowPos][colPos - 1] > garden[nextRow][nextCol]:
			nextRow = rowPos
			nextCol = colPos - 1

	if colPos + 1 < len(garden[0]):
		if nextRow == None or nextCol == None:
			nextRow = rowPos
			nextCol = colPos + 1
		elif garden[rowPos][colPos + 1] > garden[nextRow][nextCol]:
			nextRow = rowPos
			nextCol = colPos + 1

	if nextRow == None or nextCol == None:
		return yumyum

	# recur on next square
	return yumyum + rabbit(garden, nextRow, nextCol)


# garden, in this case, is a list of lists
def run(garden):
        # first find the number of rows and columns in garden to
        # determine starting point; assume each row has uniform
        # number of columns
        rows = len(garden)
        cols = len(garden[0])

        startRow = rows / 2
        startCol = cols / 2

	# gross: if we're given a matrix s.t. there are 4 possible squares to start from,
	# find the one with the max carrots and start rabbit() from there
        if rows % 2 == 0 and cols % 2 == 0:
		maxCarrots = max(garden[startRow][startCol], garden[startRow - 1][startCol],
				 garden[startRow][startCol - 1], garden[startRow - 1][ startCol - 1])

		if garden[startRow][startCol] == maxCarrots:
			return rabbit(garden, startRow, startCol)
		elif garden[startRow - 1][startCol] == maxCarrots:
                        return rabbit(garden, startRow - 1, startCol)
                elif garden[startRow][startCol - 1] == maxCarrots:
                        return rabbit(garden, startRow1, startCol - 1)
		else:
                        return rabbit(garden, startRow - 1, startCol - 1)

	# even number of rows only
	elif rows % 2 == 0:
		if garden[startRow][startCol] > garden[startRow - 1][startCol]:
			return rabbit(garden, startRow, startCol)
		else:
			return rabbit(garden, startRow - 1, startCol)

	# even number of cols only
	elif cols % 2 == 0:
		if garden[startRow][startCol] > garden[startRow][startCol - 1]:
			return rabbit(garden, startRow, startCol)
		else:
			return rabbit(garden, startRow, startCol - 1)

	# nice case: odd number of both rows and cols
	else:
		return rabbit(garden, startRow, startCol)

# example problem
l = [[5, 7, 8, 6, 3], [0, 0, 7, 0, 4], [4, 6, 3, 4, 9], [3, 1, 0, 5, 8]]
assert(run(l) == 27)

l2 = [[1, 2], [3, 4]]
assert(run(l2) == 10)

assert(run([[1]]) == 1)
assert(run([[0, 3], [1, 4]]) == 7)
