#!/usr/bin/python

# simple binary search O(log(n))
def binarySearch(L, U, lst, target):
	if L > U:
		return False

	M = (L + U)/2

	if lst[M] == target:
		return True
	else:
		if target < lst[M]:
			return binarySearch(L, M - 1, lst, target)
		else:
			return binarySearch(M + 1, U, lst, target)

def main():
	lst = [1, 2, 3, 4, 5]
	assert(binarySearch(0, len(lst) - 1 , lst, 5) == True)
	assert(binarySearch(0, len(lst) - 1 , lst, 1) == True)
	assert(binarySearch(0, len(lst) - 1 , lst, 0) == False)
	lst = [1, 4, 6, 18, 20, 29, 33]
	assert(binarySearch(0, len(lst) - 1 , lst, 5) == False)
	assert(binarySearch(0, len(lst) - 1 , lst, 29) == True)
	assert(binarySearch(0, len(lst) - 1 , lst, 2) == False)

if __name__=='__main__':
  main()

