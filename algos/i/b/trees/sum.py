from tree import *

# this is a harder problem -- I think you're dealing with a fairly dense recursive structure since
# the path doesn't have to start at the root; can turn this into a dp problem
def treesum(t, target, total):
	if t == None:
		return 0

	# if the current node's val equals the target, increast count.
	# also increase count if it's along a path that equals total
	localtotal = 0
	if total + t.val == target:
		localtotal = localtotal + 1

	return (localtotal + treesum(t.left, target, total + t.val) + treesum(t.right, target, total + t.val) +
		treesum(t.left, target, 0) + treesum(t.right, target, 0))

def local_list():
	A = Node(1)
	B = Node(2)
	C = Node(4)
	A.left = B
	A.right = C
	return A

print(treesum(local_list(), 5, 0))	# 1

def local_list1():
	A = Node(1)
	B = Node(2)
	C = Node(3)
	D = Node(0)
	E = Node(4)
	A.left = B
	A.right = C
	C.left = D
	C.right = E
	return A

print(treesum(local_list1(), 3, 0))	# 3
