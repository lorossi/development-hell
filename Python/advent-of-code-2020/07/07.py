def load_rules(path):
    with open(path, "r") as f:
        lines = f.readlines()

    return [x.strip() for x in lines]


def parse_container(rule):
    return " ".join(rule.split(" ")[:2])


def parse_contains(rule):  # sourcery skip: assign-if-exp
    remove_list = [".", "bags", "bag"]
    for r in remove_list:
        rule = rule.replace(r, "")

    right = rule.split(" contain ")[1]

    if "no other" in right:
        return []

    if "," in right:
        bags = right.split(",")
        num = [int(x.strip()[:2]) for x in bags]
        bags = [x.strip()[2:] for x in bags]
    else:
        num = [int(right.strip()[:2])]
        bags = [right.strip()[2:]]

    return [{
        "num": num[i],
        "color": bags[i]
    } for i in range(len(num))]


def parse_bags(rules):
    return [{
            "color": parse_container(r),
            "contains": parse_contains(r),
            } for r in rules]


def find_bag_by_contains(bags, contains):
    colors = []
    for b in bags:
        for c in b["contains"]:
            if contains in c["color"]:
                colors.append(b["color"])

    return colors


def find_bags_inside(bags, color, total=1):
    for b in bags:
        if b["color"] == color:
            if not b["contains"]:
                return 1

            for c in b["contains"]:
                total += c["num"] * find_bags_inside(bags, c["color"])

    return total


def main():
    rules = load_rules("bags")
    bags = parse_bags(rules)

    found = set()
    to_search = {"shiny gold"}

    while len(to_search) > 0:
        new_search = set()
        for t in to_search:
            new = find_bag_by_contains(bags, t)
            if new:
                found |= set(new)
                new_search |= set(new)

        to_search = [t for t in new_search]

    # print(found)
    # print(len(found))

    print(find_bags_inside(bags, "shiny gold", 1) - 1)


if __name__ == "__main__":
    main()
