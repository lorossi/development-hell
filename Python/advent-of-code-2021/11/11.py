def open_grid(path="input"):
    with open(path, "r") as f:
        lines = f.readlines()

    w = len(str(lines[0]).strip())
    h = len(lines)

    grid = [[0 for _ in range(w + 2)] for _ in range(h + 2)]

    for y in range(h):
        grid[y + 1] = [int(x) for x in lines[y].strip()]
        grid[y + 1].append(0)
        grid[y + 1].insert(0, 0)

    return grid


def print_grid(grid):
    print()
    for line in grid[1:-1]:
        print("".join(str(x) for x in line[1:-1]))
    print()


def check_flash(grid, w, h):
    flash = []
    for x in range(1, 1 + w):
        for y in range(1, 1 + h):
            if grid[y][x] >= 10:
                flash.append([x, y])

    return flash


def increase_grid(grid, w, h):
    for x in range(1, 1 + w):
        for y in range(1, 1 + h):
            grid[y][x] += 1


def make_flash(grid, coords):
    grid[coords[1]][coords[0]] = 0

    for dx in range(-1, 2):
        for dy in range(-1, 2):
            if dx == 0 and dy == 0:
                continue

            grid[coords[1] + dy][coords[0] + dx] += 1


def step(grid):
    w = len(grid[0]) - 2
    h = len(grid) - 2

    increase_grid(grid, w, h)

    flashes = []
    flashing = True
    while flashing:
        new_flashes = check_flash(grid, w, h)
        flashing = len(new_flashes) > 0

        for f in new_flashes:
            if f not in flashes:
                make_flash(grid, f)
                flashes.append(f)

    for f in flashes:
        grid[f[1]][f[0]] = 0

    return len(flashes)


def main():
    grid = open_grid("input")

    flashes = 0
    for _ in range(100):
        flashes += step(grid)

    print(flashes)

    w = len(grid[0]) - 2
    h = len(grid) - 2
    steps = 0
    while True:
        steps += 1
        n = step(grid)
        if n == w * h:
            break

    print(steps)


if __name__ == "__main__":
    main()
