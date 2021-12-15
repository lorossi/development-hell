from math import inf


def get_index(w, x, y):
    return w * y + x


def get_coords(w, i):
    return i % w, i // w  # x, y


def open_file(path="input"):
    with open(path, "r") as f:
        lines = f.readlines()

    return [[int(x) for x in y.strip()] for y in lines]


def expand_grid(grid):
    w, h = len(grid[0]), len(grid)
    new_grid = [[None for _ in range(h * 5)] for _ in range(w * 5)]

    for y in range(h):
        for x in range(w):
            new_grid[y][x] = grid[y][x]

    # fill first row
    for y in range(h):
        for x in range(w, w * 5):
            n = new_grid[y][x-w] + 1
            if n >= 10:
                n = 1

            new_grid[y][x] = n

    # fill the rest of the grid
    for y in range(h, 5 * h):
        for x in range(0, 5 * w):
            n = new_grid[y-h][x] + 1
            if n >= 10:
                n = 1

            new_grid[y][x] = n

    return new_grid


def print_grid(grid):
    print()
    for g in grid[1:-1]:
        print("".join(str(x) for x in g[1:-1]))
    print()


def find_neighbours(u, w, h, new=None, old=None):
    x, y = get_coords(w, u)
    neighbours = set()

    for dx in [-1, 1]:
        if x + dx < 0 or x + dx >= w:
            continue

        i = get_index(w, x+dx, y)

        if new and i in new:
            neighbours.add(i)
        if old and i not in old:
            neighbours.add(i)
        elif not old and not new:
            neighbours.add(i)

    for dy in [-1, 1]:
        if y + dy < 0 or y + dy >= h:
            continue

        i = get_index(w, x, y+dy)

        if new and i in new:
            neighbours.add(i)
        if old and i not in old:
            neighbours.add(i)
        elif not old and not new:
            neighbours.add(i)

    return neighbours


def find_closest(dist, Q):
    min_dist = inf
    closest = None

    for i, d in enumerate(dist):
        if d < min_dist and i in Q:
            min_dist = d
            closest = i

    return closest


def find_path(grid, start=None, end=None):
    w, h = len(grid[0]), len(grid)

    if not start:
        start = (0, 0)
    if not end:
        end = (w, h)

    # Dijkstra algorithm
    # that's slow as fuck but I don't know what's wrong
    dist = [inf for _ in range(w * h)]
    prev = [None for _ in range(w * h)]
    Q = {i for i in range(w * h)}
    visited = set()

    i = get_index(w, start[0], start[1])
    cut = {i}
    dist[i] = 0

    while Q:
        u = min(cut, key=lambda x: dist[x])

        Q.remove(u)
        cut.remove(u)
        visited.add(u)

        if dist[u] == inf:
            break

        neighbours = find_neighbours(u, w, h, new=Q)

        for v in neighbours:
            x, y = get_coords(w, v)
            alt = dist[u] + grid[y][x]

            if alt < dist[v]:
                dist[v] = alt
                prev[v] = u
                picked = u

        cut |= find_neighbours(picked, w, h, old=visited)

        if u == get_index(w, end[0], end[1]):
            break

    return dist


def question(grid):
    return find_path(grid)[-1]


def main():
    grid = open_file("test")
    answer = question(grid)
    print(answer)
    extended_grid = expand_grid(grid)
    answer = question(extended_grid)
    print(answer)


if __name__ == "__main__":
    main()
