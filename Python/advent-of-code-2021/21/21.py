def emulate_deterministic(p1, p2):
    counter = 0
    positions = [p1 - 1, p2 - 1]
    scores = [0, 0]

    while True:
        for p in range(2):

            roll = sum((counter + x) % 100 + 1 for x in range(3))
            counter += 3

            positions[p] = (positions[p] + roll) % 10
            scores[p] += positions[p] + 1

            if scores[p] >= 1000:
                return counter, scores


def emulate_quantum(p1, p2):
    pass


def question_one(p1, p2):
    rolls, scores = emulate_deterministic(p1, p2)
    return rolls * sorted(scores)[0]


def question_two(p1, p2):
    return max(emulate_quantum(p1, p2))


def main():
    answer = question_one(4, 8)
    print(answer)
    answer = question_two(4, 8)
    print(answer)


if __name__ == "__main__":
    main()
