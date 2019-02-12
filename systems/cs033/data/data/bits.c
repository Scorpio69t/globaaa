# define MAX 31

#if 0
INTEGER CODING RULES:

	Complete the body of each function with one
	or more lines of C code that implements the function. Your code
	must consist of a sequence of variable declarations, followed by
	a sequence of assignment statements, followed by a return statement.

	The general style is as follows:

	int funct(...) {
		/* brief description of how your implementation works */
		int var1 = expr1;
		...
		int varM = exprM;

		varJ = exprJ;
		...
		varN = exprN;

		return exprR;
	}

	Each "expr" is an expression using ONLY the following:
	1. Integer constants 0 through 255 (0xFF), inclusive. You are
	   not allowed to use big constants such as 0xFFFFFFFF.
	2. Function arguments and local variables (no global variables).
	3. Unary integer operations ! ~
	4. Binary integer operations & ^ | + << >>

	Some of the problems restrict the set of allowed operators even further.
	Each "expr" may consist of multiple operators. You are not restricted to
	one operator per line.

	You are expressly forbidden to:
	1. Use any control constructs such as if, do, while, for, switch, etc.
	2. Define or use any macros.
	3. Define any additional functions in this file.
	4. Call any functions.
	5. Use any other operations, such as &&, ||, -, or ?:
	6. Use any form of casting.
	7. Use any data type other than int.  This implies that you
	   cannot use arrays, structs, or unions.


	You may assume that your machine:
	1. Uses 2s complement, 32-bit representations of integers.
	2. Performs right shifts arithmetically.
	3. Has unpredictable behavior when shifting an integer by more
	   than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
	/*
	 * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
	 */
	int pow2plus1(int x) {
		/* exploit ability of shifts to compute powers of 2 */
		return (1 << x) + 1;
	}

	/*
	 * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
	 */
	int pow2plus4(int x) {
		/* exploit ability of shifts to compute powers of 2 */
		int result = (1 << x);
		result += 4;
		return result;
	}
#endif
/* Copyright (C) 1991-2014 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses ISO/IEC 10646 (2nd ed., published 2011-03-15) /
   Unicode 6.0.  */
/* We do not support C11 <threads.h>.  */
/*
 * bit_and - Compute x&y using only ~ and |
 *   Examples: bit_and(6, 5) = 4
 *             bit_and(3, 12) = 0
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bit_and(int x, int y) {
  // think of 'and' as the intersection of what's not in a or b
  return ~(~x | ~y);
}

/*
 * negate - Return -x
 *   Examples: negate(1) = -1
 *             negate(0) = 0
 *             negate(-33) = 33
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  // definition of two's complement
  return (~x + 1);
}

/*
 * is_equal - Return 1 if x == y, else return 0
 *   Examples: is_equal(5, 5) = 1
 *             is_equal(4, 5) = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int is_equal(int x, int y) {
  // '^' will return 0 only in the case (x == y)
  return !(x ^ y);
}

/*
 * 0div_pwr_2 - Compute x/(2^n), for 0 <= n <= 30
 *   Round toward zero
 *   Examples: div_pwr_2(15, 1) = 7
 *             div_pwr_2(-33, 4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int div_pwr_2(int x, int n) {
  /*
   * There are a couple cases. If x is positive, division
   * is simply (x >> n). If it's negative, though, if x/2^n
   * produces a remainder, the result is ((x >> n) + 1) to
   * round up towards 0.
   *
   * To find a remainder, make all bits one, then shift left
   * by n. Take its complement, which is 'less_than_2n', and bitwise 'and'
   * it with x. So, for example, say x is 00110110 and n is 2:
   *
   *			00000011	(less_than_2n)
   *		      & 00110110	(x)
   *		       ---------
   *			00000010	(remainder)
   *
   * If it's negative, and has a remainder, and n does not
   * equal 0, then add 1.
   */
  int negative = ((x >> MAX) & 1);
  int less_than_2n = ~((~0) << n);
  int remainder = !!(x & less_than_2n);

  return ((x >> n) + (negative & remainder & (!!n)));
}

/*
 * conditional - Compute the result of x ? y : z
 *   Examples: conditional(2, 4, 5) = 4
 *             conditional(0, 1, 2) = 2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  /*
   * Determine if x is true or false using (!x) and (!!x).
   * If it's true make all bits 1 s.t. (it & anything = anything).
   * If it's false, (((it & anything = 0) | something) = something).
   *
   * The shifting by MAX is simply to get 4 bytes of all 1's or 0's.
   */
  int true_c = ((!!x << MAX) >> MAX);
  int false_c = ((!x << MAX) >> MAX);

  return ((true_c & y) | (false_c & z));
}

/*
 * replaceByte(x,n,c) - Replace the n'th byte in x with given byte c.
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: replaceByte(0x12345678,1,0xab) = 0x1234ab78
 *   You can assume 0 <= n <= 3 and 0 <= c <= 255
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 3
 */
int replaceByte(int x, int n, int c) {
/*
 * 1) create 4 bytes of all ones, 2) zero the bytes in the correct
 * position using jump to locate the correct index, 3) 'and' x with
 * this new bit number to zero its bits in the n'th byte, 4) then add c
 * into this slot.
 */
  int byte_mask = 0xFF;
  int all_ones = ~0;
  int jump = (n << 3);

  return (x & (all_ones ^ (byte_mask << jump))) | (c << jump);
}

/*
 * add_ok - Return 0 if x+y will overflow, 1 otherwise
 *   Examples: add_ok(0x80000000, 0x80000000) = 0
 *             add_ok(0x80000000, 0x70000000) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int add_ok(int x, int y) {
/*
 * The only time two numbers will overflow is if they're the same sign,
 * so first check the sign. If they are the same sign, then check the last
 * bit. If x and y are positive, for example, but (x + y) < 0, then the
 * sum has overflowed. Same basic logic goes for negative numbers.
 */
  int same_sign = (((x >> MAX) ^ (y >> MAX)) ^ 1);

  return !(same_sign & ((((x + y) >> MAX) & 1) ^ ((x >> MAX) & 1)));
}

/*
 * abs_val - Return the absolute value of x
 *   Examples: abs_val(-1) = 1
 *             abs_val(33) = 33
 *   You may assume -TMax <= x <= TMax
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 4
 */
int abs_val(int x) {
/*
 * Take the 'last_bit' extended and exclusive or by it. If x is a positive number,
 * that'll be zero and return x. If it's 1, it'll do the first operation
 * of 'negating' all the bits in two's complement. Then, to complete two's
 * complement, add 1 if it's negative or zero if it's positive.
 */
  int last_bit = (x >> MAX);
  int negate = x ^ last_bit;

  return (negate + ((x >> MAX) & 1));
}

/*
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int bang(int x) {
/*
 * The only number x s.t. x and negate(x) don't have a one in the far-left
 * position is 0.
 */
  int one = ((x >> MAX) & 1);
  int negate_one = (((~x + 1) >> MAX) & 1);

  return (one | negate_one) ^ 1;
}
