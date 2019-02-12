#!/usr/bin/python

def divide(l):
  llen = len(l)
  if llen == 1 or llen == 0:
	return l
  else:
	return merge(divide(l[0:llen/2]), divide(l[(llen/2):(llen + 1)]))


def merge(l, r):
  llen = len(l)
  rlen = len(r)

  result = []

  k = 0
  i = 0

  # merge arrays without duplicates
  while(True):
	if i >= llen or k >= rlen:
		break

	if l[i] < r[k]:
	  result = result + [l[i]]
	  i = i + 1
	elif r[k] < l[i]:
	  result = result + [r[k]]
	  k = k + 1
	else:
	  result = result + [l[i]]
	  i = i + 1
	  k = k + 1

  if i < llen:
	result = result + l[i:llen + 1]
  else:
	result = result + r[k:rlen + 1]

  return result


def main():
  lst = []
  assert(divide(lst) == [])

  lst = [2]
  assert(divide(lst) == [2])

  lst = [2, 1]
  assert(divide(lst) == [1, 2])

  lst = [5, 4, 3, 2, 1]
  assert(divide(lst) == [1, 2, 3, 4, 5])

  lst = [1, 2, 3, 4, 5]
  assert(divide(lst) == [1, 2, 3, 4, 5])

  lst = [0, 3, 4, 2, 1]
  assert(divide(lst) == [0, 1, 2, 3, 4])


if __name__=='__main__':
  main()
