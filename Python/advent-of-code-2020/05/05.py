def load_passes(path):
    with open(path, "r") as f:
        lines = f.readlines()

    return [x.strip() for x in lines]


def decode_row(boarding_pass):
    row = boarding_pass[:-3].replace("B", "1").replace("F", "0")
    return int(row, 2)


def decode_col(boarding_pass):
    col = boarding_pass[-3:].replace("R", "1").replace("L", "0")
    return int(col, 2)


def decode_id(row, col):
    return row * 8 + col


def decode_passes(passes):
    ids = []
    for p in passes:
        row = decode_row(p)
        col = decode_col(p)
        ids.append(decode_id(row, col))

    return ids


def find_missing(passes):
    places = [[0 for _ in range(8)] for _ in range(128)]

    for p in passes:
        row = decode_row(p)
        col = decode_col(p)
        places[row][col] = 1

    for r in range(2, 127):
        for c in range(8):
            if places[r][c] == 0 and \
                    places[r+1][c] == 1 and places[r-1][c] == 1:
                return decode_id(r, c)


def main():
    passes = load_passes("passes")
    highest = max(decode_passes(passes))
    print(find_missing(passes))


if __name__ == "__main__":
    main()
