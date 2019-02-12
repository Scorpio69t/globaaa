#!/usr/bin/python

# takes a list of k sorted arrays of n elements each
def k_sorted(l, k, n):
  if k == 1:
    return l[0]
  else:
    res = []
    for i in range(0, k, 2):
      if i == k - 1:
        res.append(l[i])
      else:
        res.append(merge(l[i], l[i+1]))

    return k_sorted(res, len(res), 2*n)

# O(n)
def merge(l, r):
  # O(2n) = O(n)
  llen = len(l)
  rlen = len(r)
  i = 0
  j = 0
  res = []

  while(1):
    if i >= llen:
      res = res + r[j:rlen]
      break
    if j >= rlen:
      res = res + l[i:llen]
      break
    if l[i] == r[j]:
      res = res + [l[i]]
      i = i + 1
      continue
    if l[i] < r[j]:
      res = res + [l[i]]
      i = i + 1
      continue
    if l[i] > r[j]:
      res = res + [r[j]]
      j = j + 1
      continue

  return res

def main():
  assert(k_sorted([[1, 2, 3, 4], [5, 6, 7, 8]], 2, 4) == [1, 2, 3, 4, 5, 6, 7, 8])
  assert(k_sorted([[1, 5, 6, 7], [2, 3, 4, 8]], 2, 4) == [1, 2, 3, 4, 5, 6, 7, 8])
  assert(k_sorted([[0, 5, 6, 7], [2, 3, 4, 8], [1, 9, 10, 11]], 3, 4) == [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11])
  assert(k_sorted([[0, 6, 13], [2, 3, 15], [1, 9, 10]], 3, 3) == [0, 1, 2, 3, 6, 9, 10, 13, 15])
  assert(k_sorted([[1, 2, 3], [1, 2, 3]], 2, 3) == [1, 1, 2, 2, 3, 3])

if __name__ == '__main__':
  main()
