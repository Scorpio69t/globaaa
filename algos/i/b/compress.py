# Run length encoding. This should run in linear time.

def compress(s):
	result = []
	i = 0
	prev = False
	count = 0
	while True:
		if i >= len(s):
			if not prev:
				break
			else:
				result.append(prev + str(count))
				break

		# starting the compression
		if not prev:
			prev = s[i]
			count = count + 1
			i = i + 1
			continue

		if s[i] == prev:
			count = count + 1
			i = i + 1
			continue

		result.append(prev + str(count))
		prev = s[i]
		count = 1
		i = i + 1

	if len(s) <= len(result):
		return s
	else:
		# joins elements in a list
		return "".join(result)

assert(compress("") == "")
assert(compress("abcdef") == "abcdef")
assert(compress("aabcccccaaa") == "a2b1c5a3")
