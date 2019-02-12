# Linked List review

# basic singly linked list
class Node():
	def __init__(self, i):
		self.i = i
		self.nxt = None

	def next(self, n):
		self.nxt = n

class Stack():
	# implement the stack with a list
	def __init__(self):
		self.stack = []
		self.size = 0

	def push(self, e):
		self.stack.append(e)
		self.size = self.size + 1

	def pop(self):
		val = self.stack[0]
		self.stack = self.stack[1:]
		self.size = self.size - 1
		return val

def append_to_list(l, n):
	head = l
	while head.nxt != None:
		head = head.nxt

	head.next(n)

	return l

def printl(l):
	while l != None:
		print(l.i)
		l = l.nxt

# edit in place? I think if you didn't use a buffer this would be n^2 (true); use
# a hashtable instead of a list
def remove_dups(l):
	buffer = []

	if l == None:
		return
	else:
		buffer.append(l.i)

	# two nodes running in parallel
	head = l.nxt
	trailer = l
	r = trailer

	while head != None:
		if head.i in buffer:
			trailer.nxt = None
			head = head.nxt
		else:
			buffer.append(head.i)
			trailer.nxt = head
			trailer = head
			head = head.nxt

	return r

# 1st to last is the last; I like this alot, runs in linear time; always
# use hash tables!
def kth_to_last(l, k):

	if k <= 0:
		return

	d = {}
	i = 0
	while l != None:
		d[i] = l
		l = l.nxt
		i = i + 1

	# bad index
	if k > i:
		return

	return d[i - k].i

# a solution from the book; in this, I construct a tuple that i return so I can deal with the counter
# and the actual value being passed back. In the book, to have the counter persist through function
# calls they define a class that then gets passed by reference. This way if a function deep in the
# recursive call stack changes a value in the class, all other functions on the stack will see it --
# the call where counter == k can then return the head.
def wrapper(l, k):
	if l == None:
		return (0, 0)
	else:
		val = wrapper(l.nxt, k)
		val = (val[0] + 1, val[1])
		if val[0] == k:
			val = (val[0], l.i)
			return val
		else:
			return val

def rkth_to_last(l, k):
	pair = wrapper(l, k)
	return pair[1]

# v. good question; move the next node over then delete it; remember nodes
# are just their contents, so if we move their content we've effectively moved
# the node
def delete_middle(l, c):
	c.i = c.nxt.i
	c.nxt = c.nxt.nxt
	return l

def partition(head, v):
	lower = None
	upper = None
	lhead = None
	rhead = None

	while head != None:
		if head.i < v:
			if lower == None:
				lower = head
				lhead = lower
			else:
				lower.next(head)
				lower = lower.nxt
		else:
			if upper == None:
				upper = head
				rhead = upper
			else:
				upper.next(head)
				upper = upper.nxt

		head = head.nxt

	lower.next(rhead)
	upper.nxt = None

	return lhead

# interesting; in the book instead of actually creating a sum, they simply create another linked list where the position
# in the list relates to 10 ** counter.
def sum_lists(l1, l2):
	head1 = l1
	head2 = l2

	sum = 0
	counter = 0
	carry = 0

	while head1 != None:
		if head2 != None:
			val = head1.i + head2.i + carry % 10
			carry = head1.i + head2.i + carry - val
			# only interesting part; sum + 10^counter * val; adjust val due to position in list
			sum = sum + 10 ** counter * val
			counter = counter + 1
			head1 = head1.nxt
			head2 = head2.nxt
		else:
			break

	while head1 != None:
		val = (head1.i + carry) % 10
		carry = head1.i + carry - val
		sum = sum + 10 ** counter * val
		counter = counter + 1
		head1 = head1.nxt

	while head2 != None:
		val = head2.i + carry % 10
		carry = head2.i + carry - val
		sum = sum + 10 ** counter * val
		counter = counter + 1
		head2 = head2.nxt

	return sum + 10 ** counter * carry

# always use hash tables
def palindrome(l):
	d = {}
	i = 0

	while l != None:
		d[i] = l.i
		l = l.nxt
		i = i + 1

	# so if the word has 5 chars then i == 5

	for j in range(0, i/2 + 1):
		if d[j] != d[i - 1 - j]:
			return False

	return True

def palindrome_stack(l):
	head = l
	length = 0
	while head != None:
		length = length + 1
		head = head.nxt

	s = Stack()
	count = 0
	while l != None:
		if count < length / 2:
			s.push(l.i)
			l = l.nxt
			count = count + 1
			continue

		if length % 2 == 0 and count >= length / 2:
			if s.pop() != l.i:
				return False
			else:
				l = l.nxt
				count = count + 1
			continue

		if length % 2 == 1 and count == length / 2:
			count = count + 1
			l = l.nxt
			continue

		if s.pop() != l.i:
			return False
		l = l.nxt
		count = count + 1

	return True

def make_list():
	a = Node(1)
	b = Node(2)
	c = Node(2)
	d = Node(3)
	e = Node(4)
	f = Node(5)
	a.next(b)
	b.next(c)
	c.next(d)
	d.next(e)
	e.next(f)
	return a

def make_list2():
	a = Node(8)
	b = Node(6)
	c = Node(1)
	d = Node(9)
	e = Node(2)
	f = Node(0)
	a.next(b)
	b.next(c)
	c.next(d)
	d.next(e)
	e.next(f)
	return a

def make_list3():
	a = Node("a")
	b = Node("b")
	c = Node("c")
	d = Node("a")
	e = Node("b")
	a.next(b)
	b.next(c)
	c.next(d)
	d.next(e)
	return a

def run_dups():
	print("dups:")
	l = make_list()
	printl(l)
	print("---")
	remove_dups(l)
	printl(l)
	print("------------")

def run_kth():
	print("kth")
	l = make_list()
	printl(l)
	print("---")
	print(kth_to_last(l, 2))
	print("------------")

def run_remove():
	print("remove")
	l = make_list()
	printl(l)
	print("---")
	printl(delete_middle(l, l.nxt.nxt.nxt))
	print("------------")

def run_partition():
	l = make_list2()
	printl(partition(l, 6))

# 122345 + 861920 = 572389
def run_sum_lists():
	l1 = make_list()
	l2 = make_list2()
	print(sum_lists(l1, l2))

def run_palindrome():
	a = Node("a")
	b = Node("b")
	c = Node("c")
	a.next(b)
	b.next(c)

	print(palindrome(a))

def run_stack():
	s = Stack()
	s.push(1)
	s.push(2)
	s.push(3)
	assert(s.pop() == 3)
	assert(s.pop() == 2)

#run_dups()
#run_kth()
#run_remove()
#run_partition()
#run_sum_lists()
run_palindrome()
#run_stack()
assert(kth_to_last(make_list(), 2) == rkth_to_last(make_list(), 2))
assert(palindrome(make_list()) == palindrome_stack(make_list()))
assert(palindrome(make_list3()) == palindrome_stack(make_list3()))
