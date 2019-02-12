#!/usr/bin/python

from node import *

# append to the end of list
def append_to_list(l, n):
        head = l
        while head.nxt != None:
                head = head.nxt

        head.next(n)

        return l

# edit in place? I think if you didn't use a buffer this would be n^2 (true); use
# a hashtable instead of a list
def remove_dups(l):
        buffer = {}

        if l == None:
                return
        else:
		buffer[l.i] = True

        # two nodes running in parallel
        head = l.nxt
        trailer = l
        r = trailer

        while head != None:
                if head.i in buffer:
                        trailer.nxt = None
                        head = head.nxt
                else:
			buffer[head.i] = True
                        trailer.nxt = head
                        trailer = head
                        head = head.nxt
        return r

def run_remove_dups():
        print("dups:")
        l = make_list()
        printl(l)
        print("---")
        remove_dups(l)
        printl(l)
        print("------------")

run_remove_dups()
