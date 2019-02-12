#
# Find the parent process: key to this is using a hashtable and realizing all leaves of the tree
# will lead to the parent. I think this will be in the form:
#
#	[(3, [2, 4])] | 3 is the parent process of 2 and 4
#

def find_parent(l):
	d = prep_data(l)
	# if there's no keys in dict then there are no child processes
	if not d.keys():
		return (l[0])[0]

	keys = d.keys()
	key = keys[0]

	while(True):
		if key not in keys:
			return key
		else:
			key = d[key]

# place data into hashtable; you won't place the parent of the tree
# into the hashtable
def prep_data(l):
	dict = {}
	for t in l:
		for elem in t[1]:
			dict[elem] = t[0]
	return dict

assert(find_parent([(3, [])]) == 3)
assert(find_parent([(3, [2, 4])]) == 3)
assert(find_parent([(5, [18, 27]), (2, [5, 8]), (4, [9, 10]), (3, [2, 4])]) == 3)
