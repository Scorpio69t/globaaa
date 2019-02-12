# let's return the max tuple depending on its size element
def max_3(s1, s2, s3):
  sz1, _ = s1
  sz2, _ = s2
  sz3, _ = s3
  if sz1 > sz2 and sz1 > sz3:
    return s1
  elif sz2 > sz1 and sz2 > sz3:
    return s2
  else:
    return s3

# update both elements of tuple; don't use append as this will modify the list for all
# recursive calls. instead, effectively create a separate list via concatenation
def update(s1, s2, acc):
  if s1 == s2:
    sz, wrd = acc
    return (sz + 1, wrd + [s1])
  else:
    return acc

# find max length sequences
def find_seq(s1, s2, i, j, acc):
  if i == len(s1):
    return acc
  if j == len(s2):
    return acc
  return max_3(find_seq(s1, s2, i + 1, j + 1, update(s1[i], s2[j], acc)),
 	       find_seq(s1, s2, i + 1, j, acc),
	       find_seq(s1, s2, i, j + 1, acc))


def main():
  assert(find_seq([4, 5, 'v', 3, 'D', 1], [5, 4, 7, 'D', 1, 3, 120], 0, 0, (0, [])) == (3, [4, 'D', 1]))
  assert(find_seq([4, 5, 'v', 3, 'D', 1], [5, 8, 7, 'D', 1, 3, 120], 0, 0, (0, [])) == (3, [5, 'D', 1]))
  print find_seq(['A', 'G', 'C', 'G', 'A', 'G', 'A', 'G', 'A', 'G', 'T', 'G'],
		 ['C', 'G', 'A', 'T', 'C', 'G', 'A', 'T', 'T', 'T', 'A', 'G'], 0, 0, (0, []))

if __name__ == '__main__':
    main()
