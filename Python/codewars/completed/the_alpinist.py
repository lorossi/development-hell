from math import inf, isqrt


def get_index(row, col, width):
    return width * row + col


def get_coords(i, width):
    return (i // width, i % width)


def get_neighbours(i, width):
    x, y = get_coords(i, width)
    neighbours = set()

    for xx in {-1, 1}:
        if 0 <= x + xx < width:
            neighbours.add(get_index(x + xx, y, width))

    for yy in {-1, 1}:
        if 0 <= y + yy < width:
            neighbours.add(get_index(x, y + yy, width))

    return neighbours


def h(i, width):
    x, y = get_coords(i, width)
    return abs(x - width) + abs(y - width)


def path_finder(area):
    width = isqrt(len([a for a in area if a != "\n"]))
    goal = width * width - 1

    Q = set(x for x in range(width * width))
    height = {x: int(c) for x, c in enumerate(area.replace("\n", ""))}
    dist = {x: inf for x in range(width * width)}
    prev = {x: None for x in range(width * width)}

    dist[0] = height[0]

    while Q:
        # this is the most expensive step
        min_dist = inf
        u = None

        for q in Q:
            if dist[q] == inf:
                continue

            if dist[q] < min_dist:
                u = q
                min_dist = dist[q]

        Q.remove(u)

        if u == goal:
            break

        for v in get_neighbours(u, width):
            if v not in Q:
                continue

            alt = dist[u] + abs(height[u] - height[v])
            if alt < dist[v]:
                dist[v] = alt
                prev[v] = u

    current = goal
    total_price = 0

    while current:
        total_price += abs(height[current] - height[prev[current]])
        current = prev[current]

    return total_price
