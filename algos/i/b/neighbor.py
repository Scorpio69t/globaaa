#
# A neighbor game: starting out at a player and crossing out the neighbor to the right,
# then moving to the next player, who is the last player standing in this game?
#
# they're giving you an easy relation, use it; O(n)
def game(lst, start):
  dict = {}
  length = len(lst)
  i = 0

  # construct our initial dictionary
  while(i < length - 1):
    dict[lst[i]] = lst[i + 1]
    i = i + 1

  dict[lst[length - 1]] = lst[0]

  if start not in dict:
    return		# error
  else:
    while(True):
      val = dict[start]
      if val == start:
        return val
      else:
        new = dict[val]
        dict[start] = new
        del dict[val]
        start = new

assert(game([1, 2, 3, 4, 5], 3) == 5)		# return winning player


