# https://www.codewars.com/kata/5b98dfa088d44a8b000001c1/train/python

from math import inf


def flood_fill(heightmap, node=None):
    w, h = len(heightmap[0]), len(heightmap)

    if not node:
        for x in range(w):
            for y in range(h):
                if heightmap[y][x] == 0:
                    node = x, y

    x, y = node


def volume(heightmap):
    flood_fill(heightmap)
    print(heightmap)


volume([[8, 8, 8, 8, 6, 6, 6, 6],
        [8, 0, 0, 8, 6, 0, 0, 6],
        [8, 0, 0, 8, 6, 0, 0, 6],
        [8, 8, 8, 8, 6, 6, 6, 0]])
