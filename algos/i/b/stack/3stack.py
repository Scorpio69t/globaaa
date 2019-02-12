class Stack3():
	def __init__(self):
		self.stack = []
		self.size = 0
		self.one = 0
		self.two = 0
		self.three = 0

	# let's append to list so we can access elements via indices
	def increase(self, c, g):
		for i in range(c, g):
			self.size = self.size + 1
			self.stack.append(None)

	def push(self, e, stack):
		if stack == 1:
			# self.one * 3 + 1 represents the slot we want to put the item into;
			# if it's not there we need to make sure the list is long enough
			if self.size < (self.one * 3 + 1):
				self.increase(self.size, self.one * 3 + 1)

			self.stack[self.one * 3] = e
			self.one = self.one + 1

		if stack == 2:
			if self.size < self.two * 3 + 2:
				self.increase(self.size, self.two * 3 + 2)

			self.stack[self.two * 3 + 1] = e
			self.two = self.two + 1

		if stack == 3:
			if self.size < self.three * 3 + 3:
				self.increase(self.size, self.three * 3 + 3)

			self.stack[self.three * 3 + 2] = e
			self.three = self.three + 1

	def pop(self, stack):
		val = None
		if stack == 1:
			# subtract 1 from self.one since self.size is one greater (index wise)
			# than the size of the underlying list
			val = self.stack[(self.one - 1) * 3]
			self.one = self.one - 1

		if stack == 2:
			val = self.stack[(self.two - 1) * 3 + 1]
			self.two = self.two - 1


		if stack == 3:
			val = self.stack[(self.three - 1) * 3 + 2]
			self.three = self.three - 1

		return val

def test_stack():
	s = Stack3()
	s.push(5, 2)

	s.push(1, 1)
	s.push(2, 1)
	s.push(3, 1)

	s.push(10, 3)
	s.push(11, 3)

	assert(s.pop(1) == 3)
	assert(s.pop(3) == 11)
	assert(s.pop(1) == 2)
	assert(s.pop(2) == 5)

test_stack()

