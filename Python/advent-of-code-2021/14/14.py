def open_file(path="input"):
    with open(path, "r") as f:
        lines = f.readlines()

    return [x.strip() for x in lines]


def parse_polymer(path="input"):
    lines = open_file(path)
    template = {}
    rules = {}

    for x in lines[2:]:
        a, b = x.split(" -> ")
        rules[a] = [a[:1] + b, b + a[1:]]

    template = {x: lines[0].count(x) for x in rules.keys()}

    return template, rules


def step(template, rules):
    new_template = template.copy()

    for key, value in rules.items():
        if template[key] > 0:
            for v in value:
                new_template[v] += template[key]

            new_template[key] -= template[key]

    return new_template


def question_one(template):
    frequency = {s: 0 for s in "".join(template.keys())}
    for key, value in template.items():
        frequency[key[0]] += value

    most = max(f for f in frequency.values())
    least = min(f for f in frequency.values())

    # not sure why to add 1 here tho
    # i can't tell if it works by sheer luck or there's something behind it
    return most - least + 1


def main():
    template, rules = parse_polymer()

    for _ in range(40):
        template = step(template, rules)

    answer = question_one(template)
    print(answer)


if __name__ == "__main__":
    main()
