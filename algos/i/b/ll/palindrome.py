from node import *
from stack import *

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

	# find length of list; O(n)
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

		# if even and in later part of list, start popping and comparing
                if length % 2 == 0 and count >= length / 2:
                        if s.pop() != l.i:
                                return False
                        else:
                                l = l.nxt
                                count = count + 1
                        continue

		# if even and in middle, simply continue
                if length % 2 == 1 and count == length / 2:
                        count = count + 1
                        l = l.nxt
                        continue

		# else pop and compare
                if s.pop() != l.i:
                        return False
                l = l.nxt
                count = count + 1

        return True

def run_palindrome():
        a = Node("a")
        b = Node("b")
        c = Node("a")
        a.next(b)
        b.next(c)

        print(palindrome(a))

run_palindrome()
assert(palindrome(make_list()) == palindrome_stack(make_list()))
assert(palindrome(make_list3()) == palindrome_stack(make_list3()))
