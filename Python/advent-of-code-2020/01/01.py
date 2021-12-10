def load_numbers(path):
    with open(path, "r") as f:
        lines = f.readlines()

    return [int(x.strip()) for x in lines]


def find_sum_two(numbers):
    for x in numbers:
        for y in numbers:
            if x != y and x + y == 2020:
                return x, y


def find_sum_three(numbers):
    for x in numbers:
        for y in numbers:
            for z in numbers:
                if x == y == z:
                    continue

                if x + y + z == 2020:
                    return x, y, z


def main():
    numbers = load_numbers("numbers")
    x, y = find_sum_two(numbers)
    print(x, y, x * y)
    x, y, z = find_sum_three(numbers)
    print(x, y, z, x*y*z)


if __name__ == "__main__":
    main()
