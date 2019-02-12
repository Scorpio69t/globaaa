# the question for all of these trie problems: implement a data structure s.t. you can add words and then
# find partial words. if you add "add", "addition", and "all", then if you find partial "ad" = 2; find
# partial "a" = 3. this implementation is a trie with lists
def augment(trie, index):
    original = trie
    to_edit = trie[index]
    for i in range(0, 27):
        to_edit.append([])

def add(word, trie):
    original = trie
    for char in word:
        index = ord(char) - 97
        # don't need to place actual char into trie; let index represent that
        if trie[index] == []:
            augment(trie, index)
            trie = trie[index]
        else:
            trie = trie[index]

    trie[26] = '*'
    return original

def find(partial, trie):
    # go to end of partial
    for char in partial:
        if trie[ord(char) - 97] != []:
            trie = trie[ord(char) - 97]
        else:
            return 0

    return find_all(trie, False)

# need to determine the number of paths from root trie
def find_all(trie, in_word):
    if trie == [] and in_word:
        return 1
    if trie == [] and not in_word:
        return 0

    count = 0
    for i in range(0, 26):
        if trie[i] != []:
            count = count + find_all(trie[i], True)

    return (trie[26] == '*') + count

n = int(input().strip())

# create initial tries
trie = []
for i in range(0, 27):
    trie.append([])

for a0 in range(n):
    op, contact = raw_input().strip().split(' ')

    if op == "add":
        trie = add(contact, trie)
    else:
        print(find(contact, trie))
