def inArray(a, e):
  for i in range(0, len(a)):
    if a[i] == e:
      return i

  return -1

# recurrence works; just waiting to be rewritten via dynamic programming
# I think this is O(n^3) | n = len(s); iterate through every char in string
# n times, checking list l each time
def lengthHelper(s, i, j, l, m, gm):
  if i < 0 or j < 0:
    return gm

  index = inArray(l, s[j])

  # recursively call while removing from l
  if index != -1:
    gm = max(gm, m)
    return lengthHelper(s, i-1, i-1, [], 0, gm)
  else:
    gm = max(gm, m + 1)
    return lengthHelper(s, i, j-1, l + [s[j]], m + 1, gm)

def lengthOfLongestSubstring(s):
  return lengthHelper(s, len(s) - 1, len(s) - 1, [], 0, 0)

def main():
  print lengthOfLongestSubstring("abcabcbb")
  print lengthOfLongestSubstring("bbbbb")
  print lengthOfLongestSubstring("pwwkew")

if __name__=='__main__':
  main()
