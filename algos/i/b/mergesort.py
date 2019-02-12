# lol good mergesort practice; let's remove duplicates
def mergesort(l):
        if len(l) == 1 or len(l) == 0:
                return l


        left = mergesort(l[0:len(l)/2])
        right = mergesort(l[len(l)/2:])

        i = 0
        j = 0

        r = []
        while True:
                if i >= len(left) or j >= len(right):
                        break

                if left[i] < right[j]:
                        r.append(left[i])
                        i = i + 1
                        continue
                elif left[i] > right[j]:
                        r.append(right[j])
                        j = j + 1
                        continue
                else:
                        r.append(right[j])
                        i = i + 1
                        j = j + 1

        if i >= len(left):
                r.extend(right[j:])
        elif j >= len(right):
		r.extend(left[i:])

        return r

assert(mergesort([]) == [])
assert(mergesort([1, 2, 3]) == [1, 2, 3])
assert(mergesort([3, 2, 1]) == [1, 2, 3])
assert(mergesort([3, 2, 2, 2, 1]) == [1, 2, 3])
