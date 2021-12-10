def load_xmas(path):
    with open(path, "r") as f:
        lines = f.readlines()

    return [int(x.strip()) for x in lines]


def validate(slice):
    preamble = slice[:-1]
    value = slice[-1]

    for x in preamble:
        for y in preamble:
            if x == y:
                continue

            if x + y == value:
                return True

    return False


def find_first_nonvalid(xmas, preamble=5):
    count = preamble

    while count < len(xmas):
        if not validate(xmas[count-preamble:count+1]):
            return xmas[count]

        count += 1

    return None


def find_slice_of_sum(xmas, total):
    count = 0
    current_len = 2

    while count < len(xmas):
        slice = xmas[count: count + current_len - 1]

        s = sum(slice)

        if s == total:
            return min(slice), max(slice)

        if s > total:
            count += 1
            current_len = 2

        current_len += 1


def main():
    xmas = load_xmas("xmas")
    nonvalid = find_first_nonvalid(xmas, 25)
    print(nonvalid)
    min, max = find_slice_of_sum(xmas, nonvalid)
    print(min + max)


if __name__ == "__main__":
    main()
