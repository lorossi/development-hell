# https://www.codewars.com/kata/59f7597716049833200001eb


def solve(a, b):
    mirror_map = {
        "0": "0",
        "1": "1",
        "6": "9",
        "8": "8",
        "9": "6",
    }
    found = 0

    for x in range(a, b):
        original = str(x)

        try:
            reverse = "".join(mirror_map[c] for c in original[::-1])
            if reverse == original:
                found += 1
        except KeyError:
            pass

    return found
