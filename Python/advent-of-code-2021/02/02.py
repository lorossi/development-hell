def open_file(path="input"):
    with open(path, "r") as f:
        lines = f.readlines()

    return [x.strip() for x in lines]


def parse_commands_1(commands):
    x, y = 0, 0

    for c in commands:
        direction, amount = c.split(" ")

        if "forward" in direction:
            x += int(amount)
        elif "down" in direction:
            y += int(amount)
        elif "up" in direction:
            y -= int(amount)

    return x, y


def parse_commands_2(commands):
    x, y, aim = 0, 0, 0

    for c in commands:
        direction, amount = c.split(" ")

        if "forward" in direction:
            x += int(amount)
            y += aim * int(amount)
        elif "down" in direction:
            aim += int(amount)
        elif "up" in direction:
            aim -= int(amount)

    return x, y


def main():
    commands = open_file()
    x, y = parse_commands_1(commands)
    print(f"Answer to question 1: {x * y}")
    x, y = parse_commands_2(commands)
    print(f"Answer to question 2: {x * y}")


if __name__ == "__main__":
    main()
