#!/usr/bin/python

# linked list Class
class Node():
        def __init__(self, i):
                self.i = i
                self.nxt = None

        def next(self, n):
                self.nxt = n

def printl(l):
        while l != None:
                print(l.i)
                l = l.nxt

# list with duplicates
def make_list():
        a = Node(1)
        b = Node(2)
        c = Node(2)
        d = Node(3)
        e = Node(1)
        f = Node(5)
        a.next(b)
        b.next(c)
        c.next(d)
        d.next(e)
        e.next(f)
        return a

# more numbers (to add)
def make_list2():
        a = Node(8)
        b = Node(6)
        c = Node(1)
        d = Node(9)
        a.next(b)
        b.next(c)
        c.next(d)
        return a

# palindromic list
def make_list3():
        a = Node("a")
        b = Node("b")
        c = Node("a")
        d = Node("a")
        e = Node("b")
        a.next(b)
        b.next(c)
        c.next(d)
        d.next(e)
        return a
