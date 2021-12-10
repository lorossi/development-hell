
from functools import reduce


def open_file(path="input"):
    with open(path, "r") as f:
        lines = f.readlines()

    return [x.strip() for x in lines]


def first_bus(timetable):
    first_time = int(timetable[0])
    buses = [int(x) for x in timetable[1].split(",") if x != "x"]
    remainders = [b - first_time % b for b in buses]

    best_time = min(remainders)
    best_bus = remainders.index(best_time)
    return buses[best_bus], best_time


def chinese_remainder(n, a):
    sum = 0
    prod = reduce(lambda a, b: a * b, n)

    for n_i, a_i in zip(n, a):
        p = prod // n_i
        sum += a_i * mul_inv(p, n_i) * p

    return sum % prod


def mul_inv(a, b):
    b0 = b
    x0, x1 = 0, 1

    if b == 1:
        return 1

    while a > 1:
        q = a // b
        a, b = b, a % b
        x1, x1 = x1 - q * x0, x0

    if x1 < 0:
        x1 += b0

    return x1


def first_timestamp(timetable):
    buses = [int(x) for x in timetable[1].split(",") if x != "x"]
    remainders = [
        int(b) - i
        for i, b in enumerate(timetable[1].split(","))
        if b != "x"
    ]

    return chinese_remainder(buses, remainders)


def main():
    timetable = open_file()

    # id, wait = first_bus(timetable)
    # print(id * wait)

    timestamp = first_timestamp(timetable)
    print(timestamp)


if __name__ == "__main__":
    main()
