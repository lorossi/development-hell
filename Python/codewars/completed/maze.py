# https://www.codewars.com/kata/5765870e190b1472ec0022a2/train/python

from math import isqrt


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


def path_finder(maze):
    if len(maze) == 1:
        return True

    width = isqrt(len([m for m in maze if m != "\n"]))
    goal = width * width - 1

    walls = set(v for v, c in enumerate(maze.replace("\n", "")) if c == "W")
    visited = set()
    to_visit = {0}

    while to_visit:
        for n in get_neighbours(to_visit.pop(), width):
            if n in walls or n in visited:
                continue

            if n == goal:
                return True

            visited.add(n)
            to_visit.add(n)

    return False
