#!/usr/bin/python3

def substr(a, b, i, j):
	if i == len(a):
		return 0
	if j == len(b):
		return 0

	return max((a[i] == b[j]) + substr(a, b, i + 1, j + 1),
	       substr(a, b, i, j + 1),
	       substr(a, b, i + 1, j))

assert(substr("45v3D1", "", 0, 0) == 0)
assert(substr("45v3D1", "547D13120", 0, 0) == 3)
assert(substr("hello", "melodrama", 0, 0) == 3)
assert(substr("hello", "mellow", 0, 0) == 4)
assert(substr("hello", "mellowyellow", 0, 0) == 4)
assert(substr("mellowyellow", "hello", 0, 0) == 4)
