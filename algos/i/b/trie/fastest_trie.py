# no find all method; keep around metadata about the number of words added at each level
# this is the version that passes hackerrank without timing out
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
            # keep around metadata
            trie = trie[index]
            trie["size"] = trie["size"] + 1
        else:
            trie = augment(trie, index)
            trie = trie[index]
            trie["size"] = 1

    return original

def find(partial, trie):
    # go to end of partial
    for char in partial:
        index = (ord(char) - 97)
        if index in trie:
            trie = trie[index]
        else:
            return 0

    return trie["size"]

n = int(input().strip())

# create initial trie, this time as dict
trie = {}

for a0 in range(n):
    op, contact = raw_input().strip().split(' ')

    if op == "add":
        trie = add(contact, trie)
    else:
        print(find(contact, trie))
