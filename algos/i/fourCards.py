from sets import Set

funs = [(lambda x, y: x + y), (lambda x, y: x - y), (lambda x, y: y - x),
	(lambda x, y: x * y), (lambda x, y: x / y), (lambda x, y: y / x)]

# this is super gross; the idea is to go through --
def handleHash(d, o1, o2):

  for fun in funs:
    v1 = fun(o1, o2)

    for key in d.keys():

      for dfun in funs:
	v2 = dfun(key, v)

    	if d.get(v2) == None:
        	d[v2] = d[key].updateSet([Set([o1, o2])])
    	else:
        	d[v] = d[v].add([Set([o1, o2])])

    if d.get(v) == None:
	d[v] = Set([Set([o1, o2])])
    else:
	d[v] = d[v].add([Set([o1, o2])])


def cardGame(numbers):
  # dynamic hashtable
  values = {}

  for i in range(0, len(numbers)):
	for k in range(i, len(numbers)):
		handleHash(values, numbers[i], numbers[j])

lst l = [4, 1, 7, 8]

/* in this case, let's use everything */
T(i, j) = T(i - 1, j - 1) + {(L[i] + L[j]); L[i] - L[j], L[j] - L[i]; ...
	  T(i, j - 1)

(4 * (4 * 2) - 1) = C
