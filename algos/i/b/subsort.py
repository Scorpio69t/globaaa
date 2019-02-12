# don't be too quick to jump to "oh, it has to be recursive / dynamic".
# I don't think there can be multiple disparate groups, since all you have
# to do is sort one group and then the whole list should be sorted. I like
# the two pointer approach, as it approaches each sequence in turn.

# there's more structure in this problem than in like a longest subsequence
# problem; use it
def subsort(l):
        if not l:
                return (0, 0)

        i = findleft(l)
        j = findright(l)

        # it's sorted
        if i == len(l) - 1 and j == 0:
                return (0, 0)

        while i >= 0:
                if l[i] <= min(l[i + 1:]):
                        i = i + 1
                        break
                i = i - 1

        if i < 0:
                i = 0

        while j < len(l):
                if l[j] >= max(l[0:j]):
                        j = j - 1
                        break
                else:
                        j = j + 1

        if j == len(l):
                j = len(l) - 1

        return (i, j)

def findleft(l):
        for i in range(0, len(l)):
                if i == 0:
                        continue
                else:
                        if l[i] < l[i - 1]:
                                return i - 1

        return len(l) - 1

def findright(l):
        i = len(l) - 1
        while i > 0:
                if l[i] < l[i - 1]:
                        return i
                else:
                        i = i - 1
        return i

print(subsort([]))              # (0, 0)
print(subsort([0, 1]))          # (0, 0)
print(subsort([1, 0]))          # (0, 1)
print(subsort([1, 0, 2, 3]))    # (0, 1)
print(subsort([1, 2, 4, 7, 10, 11, 7, 12, 6, 7, 16, 18, 19]))   # (3, 9)
