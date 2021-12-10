def open_file(path="input"):
    with open(path, "r") as f:
        lines = f.readlines()

    return [int(x.strip()) for x in lines]


def calculate_single_depths(depths):
    deeper = 0
    for x in range(1, len(depths)):
        if depths[x] - depths[x-1] > 0:
            deeper += 1

    return deeper


def calculate_triple_depths(depths):
    deeper = 0

    for x in range(3, len(depths)):
        current = sum(depths[x-i] for i in range(3))
        prev = sum(depths[x-i-1] for i in range(3))

        if current > prev:
            deeper += 1

    return deeper


def main():
    depths = open_file("input")
    deeper = calculate_single_depths(depths)
    print(f"Answer to question 1: {deeper}")
    deeper = calculate_triple_depths(depths)
    print(f"Answer to question 2: {deeper}")


if __name__ == "__main__":
    main()
