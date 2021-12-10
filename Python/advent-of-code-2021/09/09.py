from copy import deepcopy
from functools import reduce


def load_map(path="input"):
    with open(path, "r") as f:
        lines = f.readlines()

    return [[int(x) for x in y.strip()] for y in lines]


def print_map(map):
    for line in map:
        print("".join([str(x) for x in line]))


def is_low(map, x, y, part_two=False):
    if map[y][x] == 9:
        return False
    if map[y][x] == "#":
        return True

    neighbours = find_neighbours(map, x, y)
    for n in neighbours:
        xx, yy = n
        if map[yy][xx] == "#":
            continue

        if part_two:
            if map[y][x] > map[yy][xx]:
                return False
        else:
            if map[y][x] >= map[yy][xx]:
                return False

    return True


def find_neighbours(map, x, y):
    neighbours = []

    for dy in [-1, 1]:
        if y+dy >= 0 and y+dy < len(map):
            neighbours.append((x, y+dy))

    for dx in [-1, 1]:
        if x+dx >= 0 and x+dx < len(map[y]):
            neighbours.append((x+dx, y))

    return neighbours


def find_all_lows(map):
    lows = []
    for y in range(len(map)):
        for x in range(len(map[y])):
            if is_low(map, x, y):
                lows.append((x, y))

    return lows


def mark_low(new_map, low):
    new_map[low[1]][low[0]] = "#"

    return new_map


def map_all_basins(map, lows):
    to_check = deepcopy(lows)
    visited = []
    basin_map = deepcopy(map)

    while len(to_check) > 0:
        new_check = []

        for c in to_check:
            if c in visited:
                continue

            if is_low(basin_map, c[0], c[1], True):
                mark_low(basin_map, c)
                new_check.extend(find_neighbours(map, c[0], c[1]))
                visited.append(c)

        to_check = list(set([i for i in new_check]))

    return basin_map


def find_first_basin(basins_map):
    for y in range(len(basins_map)):
        for x in range(len(basins_map[y])):
            if basins_map[y][x] == "#":
                return (x, y)


def measure_all_basins(basins_map):
    sizes = [0]
    to_check = [find_first_basin(basins_map)]
    visited = []

    # slow as fuck approach
    # but there's no way I try to optimize this
    while any("#" in line for line in basins_map):
        new_check = []

        for c in to_check:
            if c in visited:
                continue

            if basins_map[c[1]][c[0]] == "#":
                basins_map[c[1]][c[0]] = " "
                sizes[0] += 1
                new_check.extend(find_neighbours(basins_map, c[0], c[1]))
                visited.append(c)

        to_check = list(set([i for i in new_check]))

        if len(to_check) == 0:
            sizes.insert(0, 0)
            to_check = [find_first_basin(basins_map)]

    return sorted(sizes, reverse=True)


def answer_one(map):
    return sum(map[x[1]][x[0]] + 1 for x in find_all_lows(map))


def answer_two(map):
    lows = find_all_lows(map)
    basins_map = map_all_basins(map, lows)
    measures = measure_all_basins(basins_map)
    return reduce(lambda x, y: x * y, measures[:3])


def main():
    map = load_map("input")
    answer = answer_one(map)
    print(answer)
    answer = answer_two(map)
    print(answer)


if __name__ == "__main__":
    main()
