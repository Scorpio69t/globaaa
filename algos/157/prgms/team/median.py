#!/usr/bin/python

# call on list l, r, and offset
def median(l, r, offset):
  //TODO

def main():
  assert median([1, 9, 11], [10, 12], 0) == 10
  assert median([1, 2, 3, 4], [0], 0) == 2
  assert median([1, 2, 3, 4], [5], 0) == 3
  assert median([1, 2, 3, 4, 5], [], 0) == 3
  assert median([1, 2, 3, 4, 5], [6, 7, 8, 9], 0) == 5
  assert median( [5, 9, 10, 11], [1, 2, 3], 0) == 5
  assert median([6, 7, 8, 9, 10], [1, 2, 3, 4], 0) == 6
  assert median([1, 3, 6, 7, 9], [1, 4, 7, 9], 0) == 6
  assert median([1, 3, 5, 7, 10, 16, 17, 20], [-10, 5, 23], 0) == 5
  assert median([1, 9, 12], [10, 11], 0) == 10
  assert median([1, 9, 11, 12], [10], 0) == 10

if __name__ == '__main__':
  main()
