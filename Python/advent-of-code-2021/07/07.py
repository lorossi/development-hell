from functools import lru_cache


def open_file(path="input"):
    with open(path, "r") as f:
        lines = f.readlines()

    return [int(x) for x in lines[0].split(",")]


def calculate_mean(pos_list):
    return sorted(pos_list)[int(len(pos_list)/2)]


def calculate_fuel(pos_list, central):
    return sum(triang(abs(p - central)) for p in pos_list)


@lru_cache(maxsize=None)
def triang(n):
    return n * (n + 1) // 2


def move_crabs(pos_list):
    min_fuel = -1

    for i in range(max(pos_list)):
        fuel = calculate_fuel(pos_list, i)

        if min_fuel < 0 or fuel < min_fuel:
            min_fuel = fuel

    return min_fuel


def main():
    pos_list = open_file("input")
    fuel = move_crabs(pos_list)
    print(fuel)


if __name__ == "__main__":
    main()
