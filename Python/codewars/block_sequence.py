#  https://www.codewars.com/kata/5e1ab1b9fe268c0033680e5f/train/zzpython

from math import isqrt, floor


def triang(n):
    return n * (n + 1) // 2


def reverse_triang(n):
    return floor(0.5 * (isqrt(8 * n + 1) - 1))


def count_digits(n):
    if n < 10:
        return 1

    return 1 + count_digits(n // 10)


def go_back(i, n):
    pos = count_digits(i)

    while n > 1:

        while pos > 0:
            pos -= 1
            n -= 1

            if n == 1:
                break

        pos = count_digits(i) - 1
        i -= 1

    print(i, pos)
    return str(i)[pos]


def solve(n):
    if n == 1:
        return 1

    i = reverse_triang(n)
    k = 0
    while True:
        k = triang(i)

        if k >= n:
            break

        i += 1

    return go_back(i-1, k-n)


def main():
    print(solve(100), 1)
    # print(solve(2100), 2)
    # print(solve(31000), 2)


if __name__ == "__main__":
    main()
