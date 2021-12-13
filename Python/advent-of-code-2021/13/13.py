def open_file(path="input"):
    with open(path, "r") as f:
        lines = f.readlines()

    return [x.strip() for x in lines]


def parse_file(path="input"):
    lines = open_file(path)

    points = []
    folds = []

    for x in lines:
        if not x:
            continue

        if "," in x:
            points.append([int(y) for y in x.split(",")])
        else:
            coord = x.split(" ")[-1].split("=")
            folds.append((coord[0], int(coord[1])))

    w = max(x[0] for x in points)
    h = max(x[1] for x in points)

    grid = [["." for _ in range(w + 1)] for _ in range(h + 1)]

    for p in points:
        grid[p[1]][p[0]] = "#"

    return grid, folds


def print_grid(grid):
    for g in grid:
        print("".join(g))
    print()


def fold_once(grid, fold):
    dir, pos = fold

    if dir == "y":
        w = len(grid[pos])
        h = len(grid)

        for y in range(pos):
            m = h - y - 1
            for x in range(w):
                if grid[m][x] == "#":
                    grid[y][x] = "#"

        grid = grid[:pos]

    elif dir == "x":
        w = pos
        h = len(grid)

        for x in range(pos):
            m = len(grid[pos]) - x - 1
            for y in range(h):
                if grid[y][m] == "#":
                    grid[y][x] = "#"

        grid = [g[:pos] for g in grid]

    return grid


def count_pounds(grid):
    return sum(g.count("#") for g in grid)


def question_one(grid, folds):
    g = fold_once(grid, folds[0])
    return count_pounds(g)


def question_two(grid, folds):
    g = [x for x in grid]
    for f in folds:
        g = fold_once(g, f)

    # honestly, I can barely read this one
    print_grid(g)


def main():
    grid, folds = parse_file("input")

    answer = question_one(grid, folds)
    print(answer)
    question_two(grid, folds)


if __name__ == "__main__":
    main()
