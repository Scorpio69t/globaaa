def fizzbuzz(i):
	for j in range(1, i + 1):
		s = ""
		if j % 3 == 0:
			s = s + "fizz"
		if j % 5 == 0:
			s = s + "buzz"

		print(str(j) + " : " + s)

fizzbuzz(30)
