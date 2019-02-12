# O(n + m)
def check_perm(s1, s2):
  s1dict = {}
  s2dict = {}

  # O(n)
  for i in s1:
    if i in s1dict:
      s1dict[i] = s1dict[i] + 1
    else:
      s1dict[i] = 1

  # O(m)
  for i in s2:
    if i in s2dict:
      s2dict[i] = s2dict[i] + 1
    else:
      s2dict[i] = 1

  # O(n)
  for key in s1dict.keys():
    # all O(1)
    if key in s2dict:
      if s2dict[key] == s1dict[key]:
	continue
      else:
        return False
    else:
      return False

  return True

assert(check_perm("hello", "llheo"))
assert(not check_perm("hello", "lheo"))
