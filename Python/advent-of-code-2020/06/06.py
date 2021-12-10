def load_answers(path):
    with open(path, "r") as f:
        lines = f.readlines()

    return [x.strip() for x in lines]


def init_group():
    return {
        "size": 0,
        "answers": set(),
    }


def parse_groups(answers):
    groups = []

    new_group = init_group()
    for line in answers:
        if line:
            new_group["size"] += 1
            if len(new_group["answers"]) == 0 and new_group["size"] == 1:
                new_group["answers"] = set(line)
            else:
                new_group["answers"] &= set(line)
        else:
            groups.append(new_group)
            new_group = init_group()

    groups.append(new_group)
    return groups


def main():
    answers = load_answers("answers")
    groups = parse_groups(answers)

    print(sum(len(g["answers"]) for g in groups))


if __name__ == "__main__":
    main()
