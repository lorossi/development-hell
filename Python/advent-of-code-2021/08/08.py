def open_file(path="input"):
    with open(path, "r") as f:
        lines = f.readlines()

    return [x.strip() for x in lines]


def parse_signals(raw):
    return [{
        "pattern": r.split(" | ")[0].split(" "),
        "digits": r.split(" | ")[1].split(" "),
    } for r in raw]


def question_one(parsed):
    total = 0
    lengths = [2, 4, 3, 7]
    for p in parsed:
        for d in p["digits"]:
            if len(d) in lengths:
                total += 1

    return total


def find_num(digit, scrambled):
    for k, v in digit.items():
        if len(v) == len(scrambled) and sorted(v) == sorted(scrambled):
            return k


def decode_digits(parsed):
    decoded = []

    for line in parsed:
        # initialize segments dict
        segments = {
            chr(ord("a") + i): None for i in range(7)
        }
        # initialize digits dict
        digit = {
            x: None for x in range(10)
        }

        # first decode the unique numbers
        digit[1] = [d for d in line["pattern"] if len(d) == 2][0]
        digit[4] = [d for d in line["pattern"] if len(d) == 4][0]
        digit[7] = [d for d in line["pattern"] if len(d) == 3][0]
        digit[8] = [d for d in line["pattern"] if len(d) == 7][0]

        # then find the 3
        for d in line["pattern"]:
            if len(d) == 5:
                if all(s in d for s in digit[1]):
                    digit[3] = d
                    break

        # it's now possible to identify the central segment
        segments["g"] = [
            d for d in digit[4]
            if d in digit[3] and d not in digit[1]
        ][0]

        # now identify 6, 0 and 9 thanks to the central segment and to number 3
        for d in line["pattern"]:
            if len(d) == 6:
                if all(s in d for s in digit[3]):
                    digit[9] = d
                elif segments["g"] in d:
                    digit[6] = d
                else:
                    digit[0] = d
        # it's now possible to identify segments a and f (a is done later)
        segments["f"] = [d for d in digit[9] if d not in digit[3]][0]

        # nwo identify digits 5 and 2 thanks to segment f
        for d in line["pattern"]:
            if len(d) == 5:
                # 3 also is 5 segments long, but since it has already
                #  been identified, skip
                if d == digit[3]:
                    continue

                if segments["f"] in d:
                    digit[5] = d
                else:
                    digit[2] = d

        # identify all remaining segments even if it's not actually necessary
        segments["a"] = [d for d in digit[7] if d not in digit[1]][0]
        segments["b"] = [d for d in digit[1] if d not in digit[5]][0]
        segments["c"] = [d for d in digit[1] if d in digit[5]][0]
        segments["d"] = [d for d in digit[2] if d not in digit[3]][0]
        segments["e"] = [
            d for i in range(7) for d in chr(ord("a") + i)
            if d not in segments.values()
        ][0]

        # now decode the number
        decoded_number = 0
        for i, d in enumerate(line["digits"]):
            e = len(line["digits"]) - i - 1
            decoded_number += find_num(digit, d) * 10 ** e

        decoded.append(decoded_number)

    return decoded


def question_two(decoded):
    return sum(decoded)


def main():
    raw = open_file("input")
    parsed = parse_signals(raw)
    answer = question_one(parsed)
    print(answer)
    decoded = decode_digits(parsed)
    answer = question_two(decoded)
    print(answer)


if __name__ == "__main__":
    main()
