def open_file(path="input"):
    with open(path, "r") as f:
        lines = f.readlines()

    return [x.strip() for x in lines]


def calculate_answer(lines, part_one=True):
    to_replace = ["()", "[]", "{}", "<>"]
    left = [x[0] for x in to_replace]
    right = [x[1] for x in to_replace]

    def parse_line(line):
        while any(y in line for y in to_replace):
            for r in to_replace:
                line = line.replace(r, "")
        return line

    n_lines = [x for x in lines]

    if part_one:
        scores = [3, 57, 1197, 25137]
        score = 0
    else:
        scores = [1, 2, 3, 4]
        score = []

    for x in range(len(n_lines)):
        n_lines[x] = parse_line(n_lines[x])

        # the line is either wrong or incomplete
        wrong = any(r in n_lines[x] for r in right)

        if part_one:
            # is the line wrong?
            if wrong:
                for r in left:
                    n_lines[x] = n_lines[x].replace(r, "")

                i = right.index(n_lines[x][0])
                score += scores[i]

        else:
            # is the line incomplete?
            if not wrong:
                stack = [f for f in n_lines[x]]
                current_score = 0

                while len(stack) > 0:
                    i = left.index(stack.pop())
                    current_score = current_score * 5 + scores[i]

                score.append(current_score)

    if part_one:
        return score

    return sorted(score)[len(score) // 2]


def main():
    lines = open_file()
    answer = calculate_answer(lines)
    print(answer)
    answer = calculate_answer(lines, False)
    print(answer)


if __name__ == "__main__":
    main()
