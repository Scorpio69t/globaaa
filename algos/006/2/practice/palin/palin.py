# return string and length of palindrome; truly terrible running time of O(n^2)
def find_palin(start, end, l):
  while (start != end):

    i = start
    k = end

    while (i < k):
      if (l[i] != l[k]):
        break;
      i = i + 1
      k = k - 1

    if i >= k:
      return (l[start:end + 1], end - start + 1)
    else:
      end = end - 1

  return ("", 0)

# iterate over string s checking for palindromes; you'll call find_palin() n times, resulting in O(n^3)
# this can be improved with dynamic programming, which I'm not great at, yet
def longest_palindrome(s):
  length = len(s) - 1
  start = 0
  str = ''
  l = 0

  while (start != length):
    (tstr, tl) = find_palin(start, length, s)
    if tl > l:
      str = tstr
      l = tl

    start = start + 1

  return str

def main():
  assert longest_palindrome("cbbd") == "bb"
  assert longest_palindrome("babad") == "bab"
  assert longest_palindrome("thisisaracecar") == "racecar"
  assert longest_palindrome("forgeeksskeegfor") == "geeksskeeg"
  assert longest_palindrome("thihtokbyleetteelinthemiddle") == "leetteel"

  print longest_palindrome("dabcacbd")
  print longest_palindrome("localareaman")
  print longest_palindrome("toptensecretsofcs")

if __name__=='__main__':
  main()
