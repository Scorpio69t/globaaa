# taken from hackerrank: determine whether a tree is a binary tree
#
# for each node, we go down both its left and right subtrees to make sure it follows
# the invariants of a binary tree. I think this'll run in O(n^2).
#
# if we start from the root, we check that everything in its left subtree is less than the root
# and everything in its right subtree is greater than the root. when we recur on root.left, we only
# have to check its left and right subtrees since we know the root is greater than root.left and everything
# to the right is also greater than root.left (since it's greater than root)
class node:
    def __init__(self, data):
        self.data = data
        self.left = None
        self.right = None

def right_helper(right, data):
    if right == None:
        return True

    if right.data > data:
        return right_helper(right.left, data) and right_helper(right.right, data)
    else:
        return False

def left_helper(left, data):
    if left == None:
        return True

    if left.data < data:
        return left_helper(left.left, data) and left_helper(left.right, data)
    else:
        return False

def checkBST(root):
    if root == None:
        return True

    return (left_helper(root.left, root.data) and right_helper(root.right, root.data) and
            checkBST(root.left) and checkBST(root.right))
