# With a data structure; O(n)
def naive(lst, visited):
  if not lst:
    return []
  else:
    c = lst[0]
    if c in visited:
      return naive(lst[1:], visited)
    else:
      visited[c] = True
      l = naive(lst[1:], visited)
      l.append(c)
      return l

assert(naive([], {}) == [])
assert(naive([4, 4, 4], {}) == [4])
assert(naive([1, 2, 1, 3, 4, 4, 4, 5, 2], {}) == [5, 4, 3, 2, 1])

# Can also do the above with a verion of merge sort; no additional data structures, O(nlog(n))
