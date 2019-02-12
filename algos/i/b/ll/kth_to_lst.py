from node import *

# 1st to last is the last; I like this alot, runs in linear time; always
# use hash tables
def kth_to_last(l, k):

        if k <= 0:
                return

        d = {}
        i = 0
	# place elements in hash table and keep track of # of elements
        while l != None:
                d[i] = l
                l = l.nxt
                i = i + 1

        # bad index
        if k > i:
                return

	# think of base case; if I only put in a single item, i = 1, so
	# to get it out of the 0th index just subtract k
        return d[i - k].i


# a solution from the book; in this, I construct a tuple that i return so I can deal with the counter
# and the actual value being passed back. In the book, to have the counter persist through function
# calls they define a class that then gets passed by reference. This way if a function deep in the
# recursive call stack changes a value in the class, all other functions on the stack will see it --
# the call where counter == k can then return the head.
def wrapper(l, k):
        if l == None:
                return (0, None)
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

def run_kth():
        print("3rd to last element in:")
        l = make_list()
        printl(l)
        print("---")
	print("is:")
        print(kth_to_last(l, 3))
        print("------------")

run_kth()
assert(kth_to_last(make_list(), 2) == rkth_to_last(make_list(), 2))
