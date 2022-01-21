from random import choice


class Memoize:
    def __init__(self, f):
        self._f = f
        self._m = {}

    def __call__(self, args):
        if args not in self._m:
            self._m[args] = self._f(args)
        return self._m[args]


@Memoize
def first_free(row):
    if len(set(row)) == 1:
        return row[0]

    for x in {"R", "G", "B"}:
        if x not in row:
            return x


@Memoize
def triangle(row):
    m = len(row)
    # print(row, m, end="")
    # input()

    if m <= 2:
        return first_free(row)

    return triangle("".join([
        triangle(row[:-1]),  triangle(row[1:])
    ]))


def main():
    basic_cases = [
        ['B', 'B'],
        ['GB', 'R'],
        ['RRR', 'R'],
        ['RGBG', 'B'],
        ['RBRGBRB', 'G'],
        ['RRGBRGBB', 'G'],
        ['RBRGBRBGGRRRBGBBBGG', 'G']
    ]

    for b in basic_cases:
        result = triangle(b[0])
        print(result, b[1], result == b[1])

    # try to not hit recursion limit here
    for x in range(3):
        try:
            print(triangle("".join(choice(["R", "G", "B"])
                                   for _ in range(10 ** (3 + x)))))
        except RecursionError:
            print(f"RECURSION ERROR ON {10 ** (3 + x)}")


if __name__ == "__main__":
    main()
