class Stack():

        def __init__(self):
                self.stack = []
                self.size = 0

        def push(self, e):
                self.stack.append(e)
                self.size = self.size + 1

        def pop(self):
                if self.stack:
                        val = self.stack[self.size - 1]
                        self.stack = self.stack[:-1]
                        self.size = self.size - 1
                        return val
                else:
                        return None

        def peek(self):
                if self.size > 0:
                        return self.stack[self.size - 1]
                else:
                        return None
