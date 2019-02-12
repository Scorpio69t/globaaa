from tree import *

# for each node, check that the subtrees to the left and right are of the correct height
# here, you're dealing with a binary tree in which each node can have at most 2 children

def check(t):
	if t == None:
		return 0
	else:
		left = check(t.left)
		right = check(t.right)

		if left == None or right == None:
			return None

		if abs(left - right) > 1:
			return None

		# I think you want to keep around the max height
		return max(left, right) + 1


def bt(t):
	if None == check(t):
		print("Unbalanced")
	else:
		print("Balanced")


bt(make_tree())		# Unbalanced
bt(make_tree2())	# Balanced


def local_tree():
	A = Node("A")
	return A

bt(local_tree())	# Balanced


def local_tree1():
	A = Node("A")
	B = Node("B")
	A.left = B
	return A

bt(local_tree1())	# Balanced


def local_tree2():
	A = Node("A")
	B = Node("B")
	C = Node("C")
	A.left = B
	B.left = C
	return A

bt(local_tree2())	# Unbalanced


def local_tree3():
	A = Node("A")
	B = Node("B")
	C = Node("C")
	D = Node("D")
	A.left = B
	A.right = C
	B.left = D
	return A

bt(local_tree3())	# Balanced


def local_tree4():
	A = Node("A")
	B = Node("B")
	C = Node("C")
	D = Node("D")
	E = Node("E")
	A.left = B
	A.right = C
	B.left = D
	D.left = E
	return A

bt(local_tree4())	# Unbalanced
