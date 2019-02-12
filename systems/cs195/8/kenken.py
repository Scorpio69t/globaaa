#!/usr/bin/env python

from z3 import *

def get_grid(N, L, model):
    """Consumes `N` (size of the board), `L` (dict mapping indices to z3 variables), `model` (z3 model)
    and returns a grid"""

    return [[int(str(model.evaluate(L[i, j]))) for j in range(N)] for i in range(N)]

def print_grid(grid):
    """Pretty-print the grid"""

    if grid is None:
        print("No solution!")
        return

    for row in grid:
        for col in row:
            print(col, end='')
        print()

class Kenken(object):
    def __init__(self, N, game_data):
        """Constructor of this class"""

        self.N = N
        self.game_data = game_data

        # Solver
        self.s = Solver()

        # Grid Variables
        self.L = {(i, j): Int('var_{}_{}'.format(i, j)) for i in range(N) for j in range(N)}

        # If you need to initialize anything that should be shared across `solve` and `check_multiple`
        # feel free to add it here

    def solve(self):
        """PART 1"""

        # EDIT HERE: PART 1 CONSTRAINTS
        # Here, `self.N` is the size of the board, and `self.game_data` is the game data
        # Feel free to use `self.s.push()` and `self.s.pop()` across `solve` and `check_multiple`
        # to reset constraints if you don't want to reuse them in `check_multiple`

	# O(n^3) make constaints s.t. no vars along a row or column can equal each other
        for i in range(0, self.N):
                for j in range(0, self.N - 1):
                        self.s.add(self.L[(i, j)] >= 1, self.L[(i, j)] <= self.N)
                        self.s.add(self.L[(i, self.N - 1)] >= 1, self.L[(i, self.N - 1)] <= self.N)
                        for k in range(j + 1, self.N):
                                self.s.add(self.L[(i, j)] != self.L[(i, k)])
                                self.s.add(self.L[(j, i)] != self.L[(k, i)])

	# handle cages
        for cage in self.game_data:
                if cage['operation'] == "+":
                        summation = 0
                        for cell in cage['cells']:
                                summation = summation + self.L[cell]
                        self.s.add(summation == cage['number'])
                if cage['operation'] == "-":
                        self.s.add(Or(self.L[cage['cells'][0]] - self.L[cage['cells'][1]] == cage['number'],
                                      self.L[cage['cells'][1]] - self.L[cage['cells'][0]] == cage['number']))
                if cage['operation'] == "*":
                        product = 1
                        for cell in cage['cells']:
                                product = product * self.L[cell]
                        self.s.add(product == cage['number'])
		# change to multiplication to make faster
                if cage['operation'] == "/":
                        self.s.add(Or(self.L[cage['cells'][0]] / self.L[cage['cells'][1]] == cage['number'],
                                      self.L[cage['cells'][1]] / self.L[cage['cells'][0]] == cage['number']))

        # Check for PART 1
        result = self.s.check()
        if result == sat:
            return get_grid(self.N, self.L, self.s.model())
        else:
            return None

    def check_multiple(self):
        """PART 2"""

        # EDIT HERE: PART 2 CONSTRAINTS
        # Here, `self.N` is the size of the board, and `self.game_data` is the game data
        # Feel free to use `self.s.push()` and `self.s.pop()` across `solve` and `check_multiple`
        # to reset constraints if you don't want to reuse them in `check_multiple`

	# create a model, like above
        self.s.check()
        m = self.s.model()

	# push original board
        self.s.push()
        for i in range(0, self.N):
                for j in range(0, self.N):
                        self.s.add(self.L[(i, j)] != m.evaluate(self.L[(i,j)]))
			# push new constraint
                        self.s.push()
                        result = self.s.check()
                        if result == sat:
				# see what the board is
                                # print_grid(get_grid(self.N, self.L, self.s.model()))
                                self.s.pop()
                                return True
			# pop off constraint
                        self.s.pop()
        return False

if __name__ == "__main__":
    game_data_example = [
        dict(operation='+', number=11, cells=[(0, 0), (1, 0)]),
        dict(operation='/', number=2, cells=[(0, 1), (0, 2)]),
        dict(operation='*', number=20, cells=[(0, 3), (1, 3)]),
        dict(operation='*', number=6, cells=[(0, 4), (0, 5), (1, 5), (2, 5)]),
        dict(operation='-', number=3, cells=[(1, 1), (1, 2)]),
        dict(operation='/', number=3, cells=[(1, 4), (2, 4)]),
        dict(operation='*', number=240, cells=[(2, 0), (2, 1), (3, 0), (3, 1)]),
        dict(operation='*', number=6, cells=[(2, 2), (2, 3)]),
        dict(operation='*', number=6, cells=[(3, 2), (4, 2)]),
        dict(operation='+', number=7, cells=[(3, 3), (4, 3), (4, 4)]),
        dict(operation='*', number=30, cells=[(3, 4), (3, 5)]),
        dict(operation='*', number=6, cells=[(4, 0), (4, 1)]),
        dict(operation='+', number=9, cells=[(4, 5), (5, 5)]),
        dict(operation='+', number=8, cells=[(5, 0), (5, 1), (5, 2)]),
        dict(operation='/', number=2, cells=[(5, 3), (5, 4)]),
    ]

    kenken = Kenken(6, game_data_example)

    # PART 1
    print_grid(kenken.solve())

    # PART 2
    print(kenken.check_multiple())
