# bloomberg interview question: take a binary tree with a next field, and connect the
# next fields to point to the next node in that level.
from tree import *

def bfs(lst):
	while lst:
		# get next node in list
		node = lst[0]
		# update working list
		lst = lst[1:]
		# if there's something else in list
		if lst:
			next = lst[0]
			if next.level == node.level:
				node.next = next
			else:
				node.next = None
			if node.left != None:
				node.left.level = node.level + 1
				lst.append(node.left)
			if node.right != None:
				node.right.level = node.level + 1
				lst.append(node.right)
		else:
			node.next = None
			return

def create_next(t):
	# takes a tree as input, should return the tree with next pointers
	if t == None:
		return
	if t.left != None:
		t.left.level = 1
	if t.right != None:
		t.right.level = 1
	wl = [t.left, t.right]
	bfs(wl)
	return t

a = Node(1)
b = Node(2)
c = Node(3)
d = Node(4)
e = Node(5)
f = Node(6)
g = Node(7)

a.left = b
a.right = c
b.left = d
b.right = e
c.right = f
f.right = g

t = create_next(a)
assert(t.left.next.val == 3)
assert(t.left.left.next.val == 5)
assert(t.left.right.next.val == 6)
assert(t.right.next == None)
assert(t.right.right.next == None)
