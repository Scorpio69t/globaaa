# In this problem we're looking to maximize the value of the objects the robbers can steal with
# a bag that can only carry weight W = 10. This is the knapsack problem without repetition.
#
# 					items[i].value + T(items, weight - items[i].weight, i + 1)	# if weight - items[i].weight >= 0
#	T(items, weight, i) = max {														}
#					T(items, weight, i + 1)
#
class item():
	def __init__(self, weight, value):
		self.weight = weight
		self.value = value

# recursive
def knapsack(items, weight, i):
	if i >= len(items):
		return 0

	if weight - items[i].weight >= 0:
		return max(items[i].value + knapsack(items, weight - items[i].weight, i + 1), knapsack(items, weight, i + 1))
	else:
		return knapsack(items, weight, i + 1)

def dynamic_sack(items, weight):
	table = []

	# the table here is weight by items
	for i in range(0, weight + 1):
		list = []
		for j in range(0, len(items) + 1):
			list.append(0)
		table.append(list)

	# traverse table left to right, top to bottom; each cell in this implementation represents the max value that
	# can be held in a bag of i weight using j items
	for i in range(1, weight + 1):
		for j in range(1, len(items) + 1):
			# if the current item's weight is less than the capacity of the bag:
			if items[j - 1].weight <= i:
				# take the max of adding that item's value + the value in the bag with weight (i - current item's weight)
				# if there's a value here, it means that entry holds the max value for a bag holding i - current item's
				# weight. let's say i = 10 and I'm trying to add 8. I can do it since 8 < 10. I then check table entry
				# for 2. If there's a value here, I can add the two since by induction i know the table entry for 2
				# represents the max value for a bag with capacity 2.
				table[i][j] = max(items[j - 1].value + table[i - items[j - 1].weight][j - 1], table[i][j - 1])
			else:
				table[i][j] = table[i][j - 1]

	return table[weight][len(items)]

# from the book pg. 181
item1 = item(6, 30)
item2 = item(3, 14)
item3 = item(4, 16)
item4 = item(2, 9)

print(knapsack([item1, item2, item3, item4], 10, 0))	# 46
print(dynamic_sack([item1, item2, item3, item4], 10))	# 46

# let's define the recurrence for repetition of items:
#
#					# if items[i].weight <= weight
#					items[i].value + T(items, weight - items[i].weight, i + 1),
#					items[i].value + T(items, weight - items[i].weight, i),
#					T(items, weight, i + 1)
#	T(items, weight, i) = max {
#					T(items, weight, i + 1)
#
def repknapsack(items, weight, i):
	if i >= len(items):
		return 0

	if weight - items[i].weight >= 0:
		return max(items[i].value + repknapsack(items, weight - items[i].weight, i + 1),
			   items[i].value + repknapsack(items, weight - items[i].weight, i),
			   repknapsack(items, weight, i + 1))
	else:
		return repknapsack(items, weight, i + 1)

print(repknapsack([item1, item2, item3, item4], 10, 0))	# 48
