from tree import *

def postorder(t):
	if t == None:
		return

	postorder(t.left)
	postorder(t.right)
	print(t.val)

def test1():
	postorder(make_tree())

test1()
