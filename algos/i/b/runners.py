# From my bloomberg onsite: given multiple checkpoints and runners who are passing those checkpoints
# in different orders, return the top 3 at any given time.

# assume you're given a list of lists with checkpoint = index + 1
def checkpoint(lsts, top, marker):
	lsts = lsts[:marker]
	lsts.reverse()
	toplst = []
	visited = []
	while top > 0:
		for lst in lsts:
			for runner in lst:
				if runner not in visited:
					toplst.append(runner)
					visited.append(runner)
					top = top - 1
					if top == 0:
						return toplst

	return toplst

def run():
	lst0 = [1, 2, 3, 4]
	lst1 = [2, 1, 3, 4]
	lst2 = [1, 3, 2, 4]
	lst3 = [3, 1]

	l = checkpoint([lst0, lst1, lst2, lst3], 3, 4)

	print(l)

run()
