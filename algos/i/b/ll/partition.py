from node import *

def partition(head, v):
        lower = None
        upper = None
        lhead = None
        rhead = None

	# create two different lists
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

	# merge
        lower.next(rhead)
        upper.nxt = None

        return lhead

def run_partition():
        l = make_list2()
        printl(partition(l, 6))

run_partition()
