#!/usr/bin/python

def rose(array, indexArray, lower, upper, size):
  if indexArray == []:
    return []

  index = indexArray[0]

  if (index <= upper) and (index >= lower) and (index - lower) == size:
    return [index]
  elif (index <= upper) and (index >= lower) and (upper - index) == size:
    return [index]
  else:
    left = rose(array, indexArray[1:len(indexArray) + 1], lower, index - 1, size)
    right = rose(array, indexArray[1:len(indexArray) + 1], index + 1, upper, size)
    return left + right

def main():
  [v] = rose([1, 2, 3, 4, 5, 6, 7], [3, 1, 0, 4, 6], 0, 6, 2)
  assert(v == 4)

  [v] = rose([1, 2, 3, 4, 5, 6, 7], [1, 2, 3, 4, 5, 6], 0, 6, 3)
  assert(v == 3)

  assert(rose([1, 2, 3, 4, 5, 6, 7], [1, 4], 0, 6, 3) == [])

  [v] = rose([1, 2, 3, 4, 5, 6, 7], [0, 6], 0, 6, 5)
  assert(v == 6)

if __name__=='__main__':
  main()
