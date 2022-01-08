
def matmult(a=None, b=None) -> list[list[int]]:
    result = [[0 for _ in range(3)] for _ in range(3)]

    for y in range(3):
        for x in range(3):
            for k in range(3):
                result[y][x] += a[y][k] * b[k][x]

    return result


def matpow(n, m=None) -> list[list[int]]:
    if not m:
        m = [[0, 1, 1], [1, 0, 0], [0, 1, 0]]

    if n == 1:
        return m

    if n % 2 == 0:
        return matpow(n/2, m=matmult(a=m, b=m))

    return matmult(a=m, b=matpow((n-1)/2, matmult(a=m, b=m)))


def padovan(n) -> int:
    if n <= 2:
        return 1

    result = matpow(n - 2)

    return sum(result[0])


def main():
    print(padovan(1000))


if __name__ == "__main__":
    main()
