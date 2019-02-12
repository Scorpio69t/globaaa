# implementation a trie with dicts; almost x7 faster than list implementation
def augment(trie, index):
    addition = {}
    trie[index] = addition
    return trie

def add(word, trie):
    original = trie
    for char in word:
        index = ord(char) - 97
        # don't need to place actual char into trie; let index represent that
        if index in trie:
            trie = trie[index]
        else:
            trie = augment(trie, index)
            trie = trie[index]

    # this value should be safe; should never get an ordinal value > 25
    trie[26] = '*'
    return original

def find(partial, trie):
    # go to end of partial
    for char in partial:
        index = (ord(char) - 97)
        if index in trie:
            trie = trie[index]
        else:
            return 0

    return find_all(trie)

# need to determine the number of paths from root trie
def find_all(trie):
    if trie == {} or trie == '*':
        return 1

    count = 0
    for k in trie:
        count = count + find_all(trie[k])

    return count

n = int(input().strip())

# create initial trie, this time as dict -- runs almost 7 times faster than the list implementation
trie = {}

for a0 in range(n):
    op, contact = raw_input().strip().split(' ')

    if op == "add":
        trie = add(contact, trie)
    else:
        print(find(contact, trie))
