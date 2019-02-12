#!/usr/bin/python

# I think the overall complexity of this is O(3n * log(n)) = O(nlog(n))
def unique(l):
  # O(n); need this so we can halve the array
  llen = len(l)

  # if we have a list of only a single number, return it
  if llen == 0 or llen == 1:
    return l
  else:
    # t(n) = 2 * t(n/2)
    left = unique(l[0:llen/2])
    right = unique(l[llen/2:llen])

    # O(n); you have to take the lengths again since you're not
    # guaranteed the lists returning from the recursive call will be
    # the same; could be shortened if duplicates have been removed
    right_len = len(right)
    left_len = len(left)
    ret = []
    i = 0
    k = 0

    # combine while removing duplicates; constant amount of time
    while(k < right_len):
      if i >= left_len:
        ret = ret + right[k:right_len]
	break
      if left[i] < right[k]:
        ret = ret + [left[i]]
	i = i + 1
      elif left[i] > right[k]:
	ret = ret + [right[k]]
        k = k + 1
      else:
	ret = ret + [left[i]]
	i = i + 1
        k = k + 1

    if i < left_len:
      ret = ret + left[i:left_len]

    return ret

def main():
  assert unique(['u', 'n', 'i', 'q', 'u', 'e']) == ['e', 'i', 'n', 'q', 'u']
  assert unique([1, 3, 2, 1, 3]) == [1, 2, 3]
  assert unique([1, 9, 8, 8, 5, 2, 2, 3, 1, 4, 5, 5, 5, 5, 0]) == [0, 1, 2, 3, 4, 5, 8, 9]

if __name__ == '__main__':
  main()
