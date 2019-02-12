# Stack: FILO; to make it return min values, use the dynamic programming technique and
# keep around the min value after each push on the stack
class Stack():

	def __init__(self):
		self.stack = []
		self.min = None
		self.size = 0

	def push(self, e):
		self.stack.append(e)
		self.size = self.size + 2

		# store min value after each push; O(1)
		if self.min == None:
			self.min = e
			self.stack.append(e)
		else:
			if self.min < e:
				self.stack.append(self.min)
			else:
				self.stack.append(e)
				self.min = e

	def pop(self):
		if self.stack:
			val = self.stack[-2]
			self.stack = self.stack[:-2]
			self.size = self.size - 2

			# change self.min back
			if self.size != 0:
				self.min = self.stack[-1]
			else:
				self.min = None

			return val
		else:
			return None

	def minf(self):
		if self.min == None:
			return None

		return self.stack[-1]

	def peek(self):
		if self.size > 0:
			return self.stack[-2]
		else:
			return None

def test_stack():
	s = Stack()
	s.push(1)
	s.push(2)
	s.push(4)
	s.push(0)

	assert(s.minf() == 0)
	assert(s.peek() == 0)
	assert(s.pop() == 0)
	assert(s.pop() == 4)
	assert(s.minf() == 1)
	assert(s.pop() == 2)
	assert(s.pop() == 1)
	assert(s.minf() == None)

test_stack()
