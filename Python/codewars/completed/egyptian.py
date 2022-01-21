# https://www.codewars.com/kata/54f8693ea58bce689100065f/train/python
from math import ceil, gcd
from fractions import Fraction


def fibonacci(x, y):
    if x == 1:
        return [y]

    a = ceil(y / x)
    b, c = (-y) % x, y * ceil(y / x)
    d = gcd(b, c)

    if b > 1 or d > 1:
        return a, *fibonacci(b // d, c // d)

    return a, c


def decompose(n):
    f = Fraction(n)
    x, y = f.numerator, f.denominator

    if x == 0:
        return []

    if y == 1:
        return [str(x)]

    return [f"1/{n}" if n != 1 else "1" for n in fibonacci(x, y)]
