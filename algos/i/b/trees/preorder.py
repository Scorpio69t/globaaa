from tree import *

def preorder(t):
	if t == None:
		return

	print t.val
	preorder(t.left)
	preorder(t.right)

def test1():
	preorder(make_tree())

test1()
