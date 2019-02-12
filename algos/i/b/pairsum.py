from mergesort import *

# naive; O(n^2)
def pairsum(l, target):
        count = 0
        seen = {}
        for i in range(0, len(l)):
                if l[i] in seen:
                        continue
                for j in range(i + 1, len(l)):
                        if l[j] in seen:
                                continue
                        if l[i] + l[j] == target:
                                # add to dict
                                seen[l[i]] = l[j]
                                seen[l[j]] = l[i]
                                count = count + 1

        return count

# count only unique tuples
print(pairsum([1, 2, 3, 4, 5], 7))
print(pairsum([1, 1, 1, 1, 1], 2))

def pairsum2(l, target):
        l = mergesort(l)

        i = 0
        j = len(l) - 1
        count = 0
        while True:
                # just like in binary search
                if i > j:
                        break

                if l[i] + l[j] == target:
                        count = count + 1
                        i = i + 1
                        j = j - 1
                        continue
                elif l[i] + l[j] > target:
                        j = j - 1
                        continue
                else:
                        i = i + 1
                        continue

        return count

print(pairsum2([1, 2, 3, 4, 5], 7))
print(pairsum2([1, 1, 1, 1, 1], 2))

# remember -> if there's a relation, use a hashtable since lookup in it is constant. here,
# the relation is that a + b = target, so for every a just see if b is also in the hashtable
def pairsum3(l, target):
        vals = {}
        count = 0
        # place values into dict
        for i in range(0, len(l)):
                vals[l[i]] = True

        for i in range(0, len(l)):
                val = target - l[i]
                if val in vals:
                        count = count + 1

                        # no duplicates
			if vals[l[i]] == vals[val]:
	                        del vals[l[i]]
			else:
	                        del vals[l[i]]
	                        del vals[val]

        return count

print(pairsum3([1, 2, 3, 4, 5], 7))
print(pairsum3([1, 1, 1, 1, 1], 2))
