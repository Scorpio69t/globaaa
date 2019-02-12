from tree import *

# this is essentially a preorder traversal, right?
def dfs(t):
	if t != None:
		print t.val
		dfs(t.left)
		dfs(t.right)
		return
	else:
		return
