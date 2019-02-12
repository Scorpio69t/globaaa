# O(n)
def in_place(s):
  lst = s.split(" ")
  for i in range(0, len(lst) - 1):
    lst[i] = lst[i] + "%20"

  print("".join(lst))

in_place("hello there")
in_place("Mr John Smith")
