from math import log2, pow


def load_code(path="input"):
    with open(path, "r") as f:
        line = f.read().strip()

    code = bin(int(line, base=16))[2:]
    if len(code) % 4 != 0:
        code = ("0" * (len(code) % 4)) + code

    return code


def bit_mask(code, n, k=1):
    return int(code[n:n+k], 2)


def parse_literal(code):
    # literal
    current = 0
    x = 0
    while True:
        current <<= 4
        current += bit_mask(code, 7 + 5 * x, 4)

        if not bit_mask(code, 6 + 5 * x):
            return current

        x += 1


def parse_operator_by_length(code):
    sub_len = bit_mask(code, 7, 15)
    print(sub_len)


def parse_len_type(code):
    return bit_mask(code)


def parse_packet(code):
    b_version = bit_mask(code, 0, 3)
    b_type = bit_mask(code, 3, 3)

    print(f"{b_version=}, {b_type=}, ")

    if b_type == 4:
        print(parse_literal(code))
    else:
        b_len_type = bit_mask(code, 6)

        if b_len_type == 0:
            parse_operator_by_length(code)


def parse_code(code):
    pass


def main():
    code = load_code("test")
    parse_code(code)


if __name__ == "__main__":
    main()
