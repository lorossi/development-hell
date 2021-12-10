def open_file(path="input"):
    with open(path, "r") as f:
        lines = f.readlines()

    return [x.strip() for x in lines]


def find_extremes(lines):
    return [max(max(x[0], x[2]) for x in lines)][0], [max(max(x[0], x[2]) for x in lines)][0]


def format_lines(raw_lines):
    lines = []
    for line in raw_lines:
        extremes = [int(x) for x in line.replace(" -> ", ",").split(",")]
        lines.append(extremes)

    return lines


def build_grid(lines, part_2=True):
    w, h = find_extremes(lines)
    grid = [[0 for _ in range(w + 1)] for _ in range(h + 1)]

    for coords in lines:

        dx = coords[0] - coords[2]
        dy = coords[1] - coords[3]

        if coords[0] == coords[2]:
            # vertical
            x = coords[0]
            y1, y2 = sorted([coords[1], coords[3]])
            for y in range(y1, y2 + 1):
                grid[y][x] += 1

        elif coords[1] == coords[3]:
            # horizontal
            y = coords[1]
            x1, x2 = sorted([coords[0], coords[2]])
            for x in range(x1, x2 + 1):
                grid[y][x] += 1

        if part_2 and abs(dx) == abs(dy):
            x1, x2 = coords[0], coords[2]
            y1, y2 = coords[1], coords[3]

            x_dir = 1 if coords[2] > coords[0] else - 1
            y_dir = 1 if coords[3] > coords[1] else - 1

            for m in range(abs(x2-x1) + 1):

                grid[y1 + m * y_dir][x1 + m * x_dir] += 1

    return grid


def print_grid(grid):
    for g in grid:
        line = "".join([str(s) if s != 0 else "." for s in g])
        print(line)


def solution(grid):
    highest = max(max(g) for g in grid)
    return sum(g.count(x) for x in range(2, highest + 1) for g in grid)


def main():
    raw_lines = open_file("input")
    lines = format_lines(raw_lines)

    grid = build_grid(lines)

    twos = solution(grid)
    print(twos)


if __name__ == "__main__":
    main()
