# https://www.codewars.com/kata/58c5577d61aefcf3ff000081/train/python


def triangle(p):
    y = 0
    dy = 1

    while True:
        yield y
        y += dy

        if y >= p:
            dy = -1
            y -= 2

        elif y < 0:
            dy = 1
            y += 2


def position(w, h):
    x = 0
    t = iter(triangle(h))

    while True:
        y = next(t)
        yield x, y
        x = (x + 1) % w


def encode_rail_fence_cipher(string, N):
    out = [[] for _ in range(N)]
    t = iter(triangle(N))
    for s in string:
        y = next(t)
        print(y)
        out[y].append(s)

    return "".join("".join(o) for o in out)


def decode_rail_fence_cipher(string, N):
    grid = [[" " for _ in range(len(string))] for _ in range(N)]

    p = iter(position(len(string), N))
    to_read = [next(p) for _ in range(len(string))]
    to_place = sorted(to_read, key=lambda x: x[1])

    for i, p in enumerate(to_place):
        x, y = p
        grid[y][x] = string[i]

    out = [grid[y][x] for x, y in to_read]

    return "".join(out)


print(encode_rail_fence_cipher("Hello, World!", 2))
