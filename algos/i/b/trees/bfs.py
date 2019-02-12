from tree import *

def helper(wl):
	if not wl:
		return

	n = wl[0]
	if n != None:
		print n.val
		wl.append(n.left)
		wl.append(n.right)

	helper(wl[1:])

def bfs(t):
	if t == None:
		return
	else:
		print t.val
		helper([t.left, t.right])

def test1():
	bfs(make_tree())

#test1()
