from node import *

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

# 513221 + 9168 = 522389
def run_sum_lists():
        l1 = make_list()
        l2 = make_list2()
        print(sum_lists(l1, l2))

run_sum_lists()
