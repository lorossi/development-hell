def open_file(path="input"):
    with open(path, "r") as f:
        lines = f.readlines()

    return [x.strip() for x in lines]


def parse_instructions(path="input"):
    instructions = []
    lines = open_file(path)

    index = ["x", "y", "z"]
    for line in lines:
        status, coords = line.split(" ")
        ranges = [c[1:].replace("=", "").replace(
            "..", " ").split(" ") for c in coords.split(",")]

        new_instructions = {
            "coords":
                {index[i]: [int(c) for c in r] for i, r in enumerate(ranges)},
            "status": 1 if status == "on" else 0
        }

        instructions.append(new_instructions)

    return instructions


def initialize_cuboid():
    return [[[0 for _ in range(-50, 51)]
             for _ in range(-50, 51)]
            for _ in range(-50, 51)]


def list_to_range(list):
    start = list[0]
    end = list[1] + 1

    if start < -50:
        start = -50

    if end > 51:
        end = 51

    return range(start, end)


def question_one(cuboid, instructions):
    for i in instructions:
        for x in list_to_range(i["coords"]["x"]):
            for y in list_to_range(i["coords"]["y"]):
                for z in list_to_range(i["coords"]["z"]):
                    cuboid[z][y][x] = i["status"]

    return sum(
        sum(
            sum(x for x in y)
            for y in z
        ) for z in cuboid
    )


def question_two(instructions):
    return 0


def main():
    instructions = parse_instructions("test")
    cuboid = initialize_cuboid()

    answer = question_one(cuboid, instructions)
    print(answer)
    answer = question_two(instructions)
    print(answer)


if __name__ == "__main__":
    main()
