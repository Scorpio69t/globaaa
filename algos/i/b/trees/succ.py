from tree import *

# there are two cases here: 1) if there's a right subtree, you want to go down it
# and then all the way to the left and 2) if there's no right subtree you just go
# up to the parent
def find_successor(t, n):
	if n.right == None and n.parent == None:
		return None

	if n.right == None:
		print n.parent.val
		return

	n = n.right
	while n.left != None:
		n = n.left

	print n.val

# copied from tree.py so I can have access to variables
def local_tree():
        # https://en.wikipedia.org/wiki/Tree_traversal; should print$
        A = Node("A")
        B = Node("B")
        B.left = A
	A.parent = B
        D = Node("D")
        B.right = D
        C = Node("C")
        E = Node("E")
        D.left = C
        D.right = E
	C.parent = D
	E.parent = D
	D.parent = B
        F = Node("F")
        F.left = B
	B.parent = F
        G = Node("G")
        F.right = G
	G.parent = F
        I = Node("I")
        H = Node("H")
	I.parent = G
	H.parent = I
        G.right = I
        I.left = H

	return (A, H)

(t, v) = local_tree()
find_successor(t, v)
