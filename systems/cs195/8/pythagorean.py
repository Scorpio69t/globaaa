#!/usr/bin/env python

from z3 import *

class PythagoreanTriples(object):
    def __init__(self):
        """Constructor of this class"""

        # Solver
        self.s = Solver()

        # Euclid's formula variables
        self.m, self.n = Ints('m n')

        # Pythagorean triple variables
        self.x, self.y, self.z = Ints('x y z')

        # EDIT HERE: GENERAL CONSTRAINTS ON EUCLID'S FORMULA AND TRIPLES
        # Here, `self.m` and `self.n` are the variables for Euclid's formula and
        # `self.x`, `self.y`, and `self.z` are the variables for your
        # Pythagorean triple.
	# m > n > 0
        self.s.add(self.m > 0)
        self.s.add(self.n > 0)
        self.s.add(self.m > self.n)

	# (m^2 - n^2, 2mn, m^2 + n^2)
        self.s.add(2 * self.m * self.n == self.y)
        self.s.add(self.m * self.m - self.n * self.n == self.x)
        self.s.add(self.m * self.m + self.n * self.n == self.z)

	# yields pythagorean triple
        self.s.add(self.x * self.x + self.y * self.y == self.z * self.z)

    def verify(self):
        """PART 1"""

        self.s.push()

        # EDIT HERE: VERIFY EUCLID'S FORMULA
        # Here, `self.m` and `self.n` are the variables for Euclid's formula and
        # `self.x`, `self.y`, and `self.z` are the variables for your
        # Pythagorean triple.

        result = self.s.check()
        self.s.pop()
        if result == sat:
		# print out actual values
                # m = self.s.model()
                # print(m.evaluate(self.x))
                # print(m.evaluate(self.y))
                # print(m.evaluate(self.z))
                # print(m.evaluate(self.m))
                # print(m.evaluate(self.n))
                return True
        else:
                return False

    def isNonPrimitive(self):
        """PART 2"""

        self.s.push()

        # EDIT HERE: CHECK IF EUCLID'S FORMULA CAN PRODUCE A NONPRIMITIVE TRIPLE
        # Here, `self.m` and `self.n` are the variables for Euclid's formula and
        # `self.x`, `self.y`, and `self.z` are the variables for your
        # Pythagorean triple.

        self.a = Int('a')
        self.s.add(self.a > 1)

        self.s.add(self.x % self.a == 0, self.y % self.a == 0, self.z % self.a == 0)

        result = self.s.check()
        self.s.pop()
        if result == sat:
                # m = self.s.model()
                # print(m.evaluate(self.x))
                # print(m.evaluate(self.y))
                # print(m.evaluate(self.z))
                # print(m.evaluate(self.a))
                return True
        else:
                return False

if __name__ == "__main__":
    p = PythagoreanTriples()

    print(p.verify())
    print(p.isNonPrimitive())
