class Node():

	# pointers to the other nodes
	def __init__(self, i):
		self.val = i
		self.left = None
		self.right = None
		self.level = 0

	# let's keep around a bit of extra information
	def left_sum(self, i):
		self.left_sum = i

	def right_sum(self, i):
		self.right_sum = i

	def parent(self, p):
		self.parent = p

def make_tree():
        # https://en.wikipedia.org/wiki/Tree_traversal; should print in order
        A = Node("A")
        B = Node("B")
        B.left = A
        D = Node("D")
        B.right = D
        C = Node("C")
        E = Node("E")
        D.left = C
        D.right = E
        F = Node("F")
        F.left = B
        G = Node("G")
        F.right = G
        I = Node("I")
        H = Node("H")
        G.right = I
        I.left = H

	return F

def make_tree2():
        A = Node("A")
        B = Node("B")
        B.left = A
        D = Node("D")
        B.right = D
        C = Node("C")
        E = Node("E")
        D.left = C
        D.right = E
        F = Node("F")
        F.left = B
        G = Node("G")
        F.right = G
        I = Node("I")
        H = Node("H")
        G.right = I
        I.left = H
        J = Node("J")
	G.left = J

	return F
