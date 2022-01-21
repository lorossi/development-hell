# https://www.codewars.com/kata/5519a584a73e70fa570005f5/train/python

from functools import lru_cache
from math import log2


@lru_cache(maxsize=None)
def factor_2r(n):
    d = n - 1
    r = 0

    while d % 2 == 0:
        d >>= 1
        r += 1

    return d, r


@lru_cache(maxsize=None)
def pow(x, n):
    if n == 0:
        return 1
    if n == 1:
        return x

    if n % 2 == 0:
        return pow(x * x, n / 2)

    return x * pow(x * x, (n - 1) / 2)


@lru_cache(maxsize=None)
def is_prime(n, k=1000):
    if n < 2:
        return False
    if n in [2, 3]:
        return True
    if n % 2 == 0:
        return False

    # miller rabin test
    d, r, = factor_2r(n)

    for a in range(2, min(n-2, int(log2(n)) ^ 2)):
        x = pow(a, d) % n
        if x in [1, n-1]:
            continue

        for _ in range(r-1):
            x = (x * x) % n
            if x == n-1:
                continue

        return False

    return True


class Primes:
    @staticmethod
    def stream():
        n = 2
        while True:
            while not is_prime(n):
                n += 1
            yield n
            n += 1


def verify(from_n, *val):
    stream = Primes.stream()
    for _ in range(from_n):
        next(stream)
    for v in val:
        print(next(stream), v)


verify(0, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29)
verify(10, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71)
verify(100, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601)
verify(1000, 7927, 7933, 7937, 7949, 7951, 7963, 7993, 8009, 8011, 8017)
