# Taken from hacker rank: determine the min number of deletions s.t. the two words are anagrams
# of each other.
#
# Should run in something close to 2(n + m) = O(n + m) | n = len(a) and m = len(b)
def number_needed(a, b):
    num = 0

    # O(n)
    adict = {}
    for c in a:
        if c in adict:
            adict[c] = 1 + adict[c]
        else:
            adict[c] = 1

    # O(m)
    bdict = {}
    for c in b:
        if c in bdict:
            bdict[c] = 1 + bdict[c]
        else:
            bdict[c] = 1

    to_del = []
    # O(n)
    for key in adict:
        if key in bdict and adict[key] == bdict[key]:
            to_del.append(key)
            continue
        if key not in bdict:
            num = num + adict[key]
        else:
            num = num + abs(adict[key] - bdict[key])
            to_del.append(key)

    # O(m)
    for elem in to_del:
        del bdict[elem]

    for key in bdict:
            num = num + bdict[key]

    return num

a = input().strip()
b = input().strip()

print(number_needed(a, b))
