from tree import *
from bfs import bfs

def create_tree(l):
	if not l:
		return
	else:
		# find the middle node; this is essentially a form of
		# binary search
		n = l[len(l)/2]

		n.left = create_tree(l[0:len(l)/2])
		n.right = create_tree(l[len(l)/2 + 1:])
		return n

# easy case
def make_list():
	A = Node("A")
	return [A]

#bfs(create_tree(make_list()))

def make_list2():
	A = Node("A")
	B = Node("B")
	C = Node("C")
	D = Node("D")
	E = Node("E")
	F = Node("F")
	G = Node("G")
	return [A, B, C, D, E, F, G]

bfs(create_tree(make_list2()))

def make_list3():
	A = Node("A")
	B = Node("B")
	C = Node("C")
	return [A, B, C]

#bfs(create_tree(make_list3()))
