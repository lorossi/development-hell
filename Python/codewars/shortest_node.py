# https://www.codewars.com/kata/5573f28798d3a46a4900007a/train/python

from math import inf


def find_neighbours(x, y, w, h):
    neighbours = []

    for xx in [-1, 1]:
        if x + xx > 0 and x + xx < w:
            neighbours.append((x + xx, y))

    for yy in [-1, 1]:
        if y + yy > 0 and y + yy < h:
            neighbours.append((x, y + yy))

    return neighbours


def heuristic(cx, cy, ex, ey):
    return abs(cy - ey) + abs(cx - ex)


def find_shortest_path(grid, start_node, end_node):
    if not grid:
        return []

    if len(grid) == 1:
        return [grid[0][0]]

    h, w = len(grid[0]), len(grid)
    print(w, h)

    # declarations
    to_visit = set()
    previous = [[None for _ in range(w)] for _ in range(h)]
    g_score = [[inf for _ in range(w)] for _ in range(h)]
    f_score = [[inf for _ in range(w)] for _ in range(h)]
    # initializations
    sx, sy = start_node.position.x, start_node.position.y
    ex, ey = end_node.position.x, end_node.position.y
    to_visit.add((sx, sy))
    previous[sx][sy] = 0
    g_score[sx][sy] = 0
    f_score[sx][sy] = 0

    while to_visit:
        min_score = inf
        current = None

        for o in to_visit:
            x, y = o

            if not grid[x][y].passable:
                continue

            if f_score[x][y] < min_score:
                min_score = f_score[x][y]
                current = (x, y)

        if current == (end_node.position.x, end_node.position.y):
            break

        to_visit.remove(current)

        cx, cy = current
        for n in find_neighbours(*current, w, h):
            nx, ny = n

            if not grid[nx][ny].passable:
                continue

            alt = g_score[cx][cy] + 1
            if alt < g_score[nx][ny]:
                previous[nx][ny] = current
                g_score[nx][ny] = alt
                f_score[nx][ny] = alt + heuristic(nx, ny, ex, ey)

                if (nx, ny) not in to_visit:
                    to_visit.add((nx, ny))

    path = []
    while True:
        cx, cy = current
        path.insert(0, grid[cx][cy])
        current = previous[cx][cy]

        if not current:
            break

    return path
