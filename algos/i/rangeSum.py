total = 0

# O(log(n))
def divide(nums, lower, upper):
  if nums == []:
    return []

  if len(nums) == 1:
    if nums[0] <= upper and nums[0] >= lower:
      global total
      total = total + 1
      return nums
    else:
      return nums

  return detRanges(divide(nums[0:len(nums)/2], lower, upper), divide(nums[len(nums)/2: len(nums) + 1], lower, upper), lower, upper)

# O(n)
def sum(l):
  sum = 0
  for i in range(0, len(l)):
    sum = sum + l[i]

  return sum

# O(n)
def detRanges(l, r, lower, upper):
  lsum = sum(l)
  for j in range(0, len(r)):
	lsum = lsum + r[j]
	if lsum >= lower and lsum <= upper:
	  global total
	  total = total + 1

  return l + r

def countRangeSum(nums, lower, upper):
  divide(nums, lower, upper)
  global total
  tmp = total
  total = 0
  return tmp

def main():
  print countRangeSum([-2, 5, -1], -2, 2)
  print countRangeSum([-1, 2, 4, -6, 8, 9], 1, 5)	# [-1, 2]; [-1, 2, 4]; [-6, 8]; [2]; [4]: 5
  print countRangeSum([-1, -2, 10, 9], 1, 5)		# 0

if __name__=='__main__':
  main()
