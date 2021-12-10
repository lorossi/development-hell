from math import sqrt, pow, floor


def load_map(path="input"):
    with open(path, "r") as f:
        lines = f.readlines()

    h, w = len(lines), len(lines[0].strip())

    initial_map = [[None for _ in range(w)] for _ in range(h)]

    for y in range(h):
        current_line = [c for c in lines[y]]
        for x in range(w):
            initial_map[y][x] = current_line[x]

    return initial_map


def get_map_size(current_map):
    return len(current_map[0]), len(current_map)


def print_map(current_map):
    for line in current_map:
        print("".join(line))
    print()


def copy_map(current_map):
    w, h = get_map_size(current_map)
    return [[current_map[y][x] for x in range(w)] for y in range(h)]


def compare_maps(m1, m2):
    w, h = get_map_size(m1)

    for x in range(w):
        for y in range(h):
            if m1[y][x] != m2[y][x]:
                return False

    return True


def count_occupied_around(current_map, x, y):
    w, h = get_map_size(current_map)

    occupied = 0
    for xx in range(x-1, x+2):
        for yy in range(y-1, y+2):
            if xx == x and yy == y:
                continue

            if xx >= w or xx < 0:
                continue
            if yy >= h or yy < 0:
                continue

            if current_map[yy][xx] == "#":
                occupied += 1

    return occupied


def get_row(current_map, x, y):
    w, _ = get_map_size(current_map)
    occupied = 0

    xx = x
    while xx > 0:
        xx -= 1

        if current_map[y][xx] == "#":
            occupied += 1
            break
        if current_map[y][xx] == "L":
            break
    xx = x
    while xx < w - 1:
        xx += 1

        if current_map[y][xx] == "#":
            occupied += 1
            break
        if current_map[y][xx] == "L":
            break

    return occupied


def get_col(current_map, x, y):
    _, h = get_map_size(current_map)
    occupied = 0

    yy = y
    while yy > 0:
        yy -= 1

        if current_map[yy][x] == "#":
            occupied += 1
            break
        if current_map[yy][x] == "L":
            break

    yy = y
    while yy < h - 1:
        yy += 1

        if current_map[yy][x] == "#":
            occupied += 1
            break
        if current_map[yy][x] == "L":
            break

    return occupied


def get_right_diagonal(current_map, x, y):
    w, h = get_map_size(current_map)
    occupied = 0

    xx, yy = x, y
    while xx > 0 and yy > 0:
        xx -= 1
        yy -= 1

        if current_map[yy][xx] == "#":
            occupied += 1
            break
        if current_map[yy][xx] == "L":
            break

    xx, yy = x, y
    while xx < w - 1 and yy < h - 1:
        xx += 1
        yy += 1

        if current_map[yy][xx] == "#":
            occupied += 1
            break
        if current_map[yy][xx] == "L":
            break

    return occupied


def get_left_diagonal(current_map, x, y):
    w, h = get_map_size(current_map)
    occupied = 0

    xx, yy = x, y
    while xx < w - 1 and yy > 0:
        xx += 1
        yy -= 1

        if current_map[yy][xx] == "#":
            occupied += 1
            break
        if current_map[yy][xx] == "L":
            break

    xx, yy = x, y
    while xx > 0 and yy < h - 1:
        xx -= 1
        yy += 1

        if current_map[yy][xx] == "#":
            occupied += 1
            break
        if current_map[yy][xx] == "L":
            break

    return occupied


def count_occupied_visible(current_map, x, y):
    occupied = 0

    occupied += get_row(current_map, x, y)
    occupied += get_col(current_map, x, y)
    occupied += get_left_diagonal(current_map, x, y)
    occupied += get_right_diagonal(current_map, x, y)

    return occupied


def count_occupied(current_map):
    w, h = get_map_size(current_map)
    return sum(1 if current_map[y][x] == "#" else 0 for x in range(w) for y in range(h))


def evaluate_next_1(current_map):
    w, h = get_map_size(current_map)

    next_map = [[0 for _ in range(w)] for _ in range(h)]
    occupied_map = [[count_occupied_around(current_map, x, y)
                    for x in range(w)] for y in range(h)]

    for x in range(w):
        for y in range(h):
            occupied = occupied_map[y][x]

            if current_map[y][x] == ".":
                next_map[y][x] = "."
            elif current_map[y][x] == "L" and occupied == 0:
                next_map[y][x] = "#"
            elif current_map[y][x] == "#" and occupied >= 4:
                next_map[y][x] = "L"
            else:
                next_map[y][x] = current_map[y][x]

    return next_map


def evaluate_next_2(current_map):
    w, h = get_map_size(current_map)

    next_map = [[0 for _ in range(w)] for _ in range(h)]
    occupied_map = [[count_occupied_visible(current_map, x, y)
                    for x in range(w)] for y in range(h)]

    for x in range(w):
        for y in range(h):
            occupied = occupied_map[y][x]

            if current_map[y][x] == ".":
                next_map[y][x] = "."
            elif current_map[y][x] == "L" and occupied == 0:
                next_map[y][x] = "#"
            elif current_map[y][x] == "#" and occupied >= 5:
                next_map[y][x] = "L"
            else:
                next_map[y][x] = current_map[y][x]

    return next_map


def main():
    previous_map = load_map()
    # current_map = evaluate_next_1(previous_map)

    # while not compare_maps(previous_map, current_map):
    #     previous_map = copy_map(current_map)
    #     current_map = evaluate_next_1(current_map)

    # occupied = count_occupied(current_map)
    # print(occupied)

    current_map = evaluate_next_2(previous_map)

    while not compare_maps(previous_map, current_map):
        previous_map = copy_map(current_map)
        current_map = evaluate_next_2(current_map)

    occupied = count_occupied(current_map)
    print(occupied)


if __name__ == "__main__":
    main()
