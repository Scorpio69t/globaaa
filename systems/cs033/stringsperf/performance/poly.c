/*
 * Brown University - Department of Computer Science
 * CS033 - Introduction To Computer Systems - Fall 2015
 * Prof. Thomas W. Doeppner
 * Assignment 6 - Performance 1: Polynomials - poly.c
 * Due date: 10/20/15
 */

/*              _
 *  _ __   ___ | |_   _
 * | '_ \ / _ \| | | | |
 * | |_) | (_) | | |_| |
 * | .__/ \___/|_|\__, |
 * |_|            |___/
 */

#include <assert.h>
#include <string.h>

extern double poly_naive(double[], double, int);

/*
 * poly() - abbreviated from "polynomial evaluation".
 * - Description: evaluate a polynomial whose coefficients are
 *   given by array a, x value by argument x, and degree degree.
 *
 * - Arguments: a, an array of ints, x, the starting x value, and degree,
 *   the degree to use when composing polynomials
 *
 * - Return value: the value of the evaluated polynomial
 *
 * TODO: Make this as fast as possible (and correct)!
 */

double poly(double a[], double x, int degree) {

  // declare variables to be used in unrolling
  double r1 = x * a[degree];
  double result = r1 + a[degree - 1];
  double r3 = 0;
  double r4 = 0;
  double r5 = 0;
  double r6 = 0;
  double y = 0;

  for(degree -= 2; degree >= 0; degree -= 4) {

    // use switch statement since it can, in some cases, be faster than if-blocks
    switch(degree) {
      case(0):
        result = (x * result) + a[degree];
        break;
      case(1):
        r3 = a[degree - 1];
        r4 = x * a[degree];
        y = x * x;
        r1 = r3 + r4;
        result = (y * result) + r1;
        break;
      case(2):
        r3 = a[degree - 2];
        r4 = x * a[degree - 1];
        r5 = x * x * a[degree];
        y = x * x * x;
        r1 = r3 + r4 + r5;
        result = (y * result) + r1;
        break;
      default:
        // unroll loop
        r3 = a[degree - 3];
        r4 = x * a[degree - 2];
        r5 = x * x * a[degree - 1];
        r6 = x * x * x * a[degree];

        // update result, multiplying result by x's consumed while unrolling
        y = x * x * x * x;
        r1 = r3 + r4 + r5 + r6;
        result = (y * result) + r1;
    }
  }

  return result;
}
