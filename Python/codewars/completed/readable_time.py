# https://www.codewars.com/kata/52742f58faf5485cae000b9a/

from math import prod


def format_duration(seconds):
    if seconds == 0:
        return "now"

    increments = [365, 24, 60, 60, 1]
    names = ["year", "day", "hour", "minute", "second"]
    values = [0 for _ in increments]

    for i in range(5):
        prev = sum(values[x] * prod(increments[x:]) for x in range(0, i))
        div = prod(increments[i:])
        values[i] = (seconds - prev) // div

    out = [" ".join(str(y) for y in z) + ("s" if z[0] > 1 else "")
           for z in zip(values, names) if z[0]]

    if len(out) == 1:
        return out[0]

    separators = ["," for _ in out]
    separators[-2] = " and"

    return " ".join([f"{x[1]}{x[0]}" for x in zip(separators, out)])[:-1]
