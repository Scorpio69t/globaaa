# let's say you're given a list of tokens that are strings; I feel like this is essentially
# using something like a lookahead; so kind of like parsing with lalr(1), keep a lookahead
# token and only start evaluating when we know the lookahead doesn't have a higher priority
# on the operand
def eval(l):

	result = 0	# for everything
	operand = 0
	op = None
	lookahead = None
	special = None	# for quotient and product

	while l:
		popped = l[0]

		if popped == "+":
			if op != None:
				if special != None:
					result = handle(result, op, special)
					special = None
				else:
					result = handle(result, op, operand)
				op = "+"
			else:
				op = "+"

			l = l[1:]
			continue

		elif popped == "-":
			if op != None:
				if special != None:
					result = handle(result, op, special)
					special = None
				else:
					result = handle(result, op, operand)
				op = "-"
			else:
				op = "-"

			l = l[1:]
			continue

		elif popped == "*":
			if special == None:
				special = operand
			operand = 0
			lookahead = popped
			l = l[1:]
			continue

		elif popped == "/":
			if special == None:
				special = operand
			operand = 0
			lookahead = popped
			l = l[1:]
			continue

		# it's a number
		else:
			# None for the first number in the expression
			if op == None:
				op = "+"
				operand = int(popped)
				l = l[1:]
				continue
			else:
				if lookahead != None:
					special = handle(special, lookahead, popped)
					lookahead = None
					l = l[1:]
					continue
				else:
					operand = int(popped)
					l = l[1:]
					continue

	if special != None:
		return handle(result, op, special)
	else:
		return handle(result, op, operand)

def handle(result, op, operand):
	if op == None:
		return result + operand
	if op == "+":
		return result + operand

	if op == "-":
		return result - operand

	if op == "*":
		return result * int(operand)

	if op == "/":
		return result / int(operand)

print(eval(["1"]))						# 1
print(eval(["-", "1"]))						# -1
print(eval(["1", "+", "2"]))					# 3
print(eval(["-", "1", "+", "2"]))				# 1
print(eval(["2", "*", "2"]))					# 4
print(eval(["1", "+", "2", "*", "2"]))				# 5
print(eval(["1", "+", "2", "*", "2", "-", "3"])) 		# 2
print(eval(["1", "+", "2", "*", "2", "-", "3", "/", "2"])) 	# 4
print(eval(["2", "*", "8", "/", "4"])) 				# 4
