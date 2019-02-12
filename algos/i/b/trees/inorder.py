from tree import *

# if there's nothing to the left, print since it's farthest to the left. Else,
# recur on the left subtree, then print out the current node, then recur on
# the right subtree
def inorder(t):
	if t == None:
		return
	if t.left == None:
		print t.val
		inorder(t.right)
	else:
		inorder(t.left)
		print t.val
		inorder(t.right)

def test1():
	inorder(make_tree())

test1()
