# https://www.codewars.com/kata/5abeaf0fee5c575ff20000e4/train/python
from math import inf


def get_neighbours(i, w, h):
    x, y = index_to_xy(i, w)

    neighbours = []
    for xx in [-1, 1]:
        if xx + x < 0 or xx + x >= w:
            continue

        neighbours.append(xy_to_index(x + xx, y, w))

    for yy in [-1, 1]:
        if yy + y < 0 or yy + y >= h:
            continue

        neighbours.append(xy_to_index(x, y + yy, w))

    return neighbours


def Dijkstra(s, f, w, h, cost):
    dist = [inf if i != s else 0 for i in range(w * h)]
    prev = [None if i != s else 0 for i in range(w * h)]
    Q = {i for i in range(w * h)}

    while Q:
        min_dist = inf

        for q in Q:
            if dist[q] < min_dist:
                min_dist = dist[q]
                u = q

        Q.remove(u)

        for v in get_neighbours(u, w, h):

            if v not in Q:
                continue

            x, y = index_to_xy(v, w)

            alt = dist[u] + cost[y][x]
            if alt < dist[v]:
                dist[v] = alt
                prev[v] = u

        if u == f:
            break

    return dist, prev


def xy_to_index(x, y, w):
    return x + w * y


def index_to_xy(i, w):
    return (i % w, i // w)


def cheapest_path(toll, start, finish):
    if start == finish:
        return []

    w, h = len(toll[0]), len(toll)
    s = xy_to_index(start[1], start[0], w)
    f = xy_to_index(finish[1], finish[0], w)
    _, prev = Dijkstra(s, f, w, h, toll)

    path = []
    current = f
    x, y = index_to_xy(current, w)
    while current != s:
        p = prev[current]
        px, py = index_to_xy(p, w)

        if py < y:
            path.insert(0, "down")
        elif py > y:
            path.insert(0, "up")
        elif px < x:
            path.insert(0, "right")
        elif px > x:
            path.insert(0, "left")

        current = p
        x, y = px, py

    return path
