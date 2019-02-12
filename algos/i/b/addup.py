#
# Questions: Given a list of numbers, take the product of all entries except the current
# entry you're on.
#
# O(n^2):
def addup(lst):
  result = []
  length = len(lst)
  for i in range(0, length):
    sum = 0
    first = True
    for j in range(0, length):
      if i == j:
	continue
      elif first:
	first = False
        sum = lst[j]
      else:
        sum = sum * lst[j]

    result.append(sum)

  return result

assert(addup([1, 2, 3, 4]) == [24, 12, 8, 6])
assert(addup([1, 0, 3, 4]) == [0, 12, 0, 0])

def addup2(lst):
  result = []
  length = len(lst)
  for i in range(0, length):
    j = (i + 1) % length
    first = True	# this is the trick; you need to denote the first thing in the lst since you're adding this to sum
    sum = 0
    while (j != i):
      if first:
	first = False
        sum = lst[j]
      else:
	sum = sum * lst[j]

      j = (j + 1) % length

    result.append(sum)

  return result

assert(addup2([1, 2, 3, 4]) == [24, 12, 8, 6])
assert(addup2([1, 0, 3, 4]) == [0, 12, 0, 0])
