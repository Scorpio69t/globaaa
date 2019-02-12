def addOperators(num, target):
  lst = split(num, target)
  i = 0

  while(True):
    if i >= len(lst):
      break

    if eval(lst[i].split(" ")) == target:
      i = i + 1
      continue
    else:
      lst = lst[0:i] + lst[i + 1:len(lst) + 1]

  return lst

def split(num, target):
  # base case
  nlen = len(num)
  if nlen == 1 or nlen == 0:
 	return num

  return combine(split(num[0:nlen/2], target), split(num[nlen/2:nlen + 1], target))

def combine(n1, n2):
  result = []
  for i in range(0, len(n1)):
	for j in range(0, len(n2)):
		result = result + [n1[i]] + [n2[j]]
		result = result + [n1[i] + " + " + n2[j]]
		result = result + [n1[i] + " - " + n2[j]]
		result = result + [n1[i] + " * " + n2[j]]

  return result

# pass into this a list
def eval(slst):
  # base cases
  if slst == []:
    return 0

  if len(slst) == 1:
    return int(slst[0])

  num = slst[0]
  operator = slst[1]
  slst = slst[2:len(slst) + 1]

  if operator == '+':
    return int(num) + eval(slst)
  elif operator == '-':
    return int(num) - eval(slst)
  else:
    return int(num) * eval(slst)

def main():
  print addOperators("123", 6)
  print addOperators("232", 8)
  print addOperators("105", 5)
  print addOperators("00", 0)
  print addOperators("3456237490", 9191)

if __name__=='__main__':
  main()
