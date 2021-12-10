def load_instructions(path):
    with open(path, "r") as f:
        lines = f.readlines()

    return [x.strip() for x in lines]


def parse_instructions(lines):
    instructions = []
    for x in lines:
        words = x.split(" ")
        opcode, arg = words[0], int(words[1])
        instructions.append({
            "opcode": opcode,
            "argument": arg,
            "visited": False,
        })
    return instructions


def reset_instructions(instructions):
    for x in instructions:
        x["visited"] = False


def emulate_instructions(instructions):
    acc = 0
    pc = 0

    while pc < len(instructions):
        current = instructions[pc]

        if current["visited"]:
            break

        current["visited"] = True

        if current["opcode"] == "nop":
            pc += 1
            continue

        if current["opcode"] == "acc":
            acc += current["argument"]
            pc += 1
            continue

        if current["opcode"] == "jmp":
            pc += current["argument"]
            continue

    return acc, pc == len(instructions)


def fix_line(instructions, count):
    current = instructions[count]

    if current["opcode"] == "nop":
        current["opcode"] = "jmp"
        return instructions

    if current["opcode"] == "jmp":
        current["opcode"] = "nop"
        return instructions

    return None


def main():
    lines = load_instructions("instructions")
    instructions = parse_instructions(lines)

    ended = False
    new_instructions = [i for i in instructions]
    count = 0

    while True:
        acc, ended = emulate_instructions(new_instructions)

        if ended:
            break

        while True:
            instructions = parse_instructions(lines)  # lame
            new_instructions = fix_line(instructions, count)
            count += 1

            if new_instructions:
                instructions = [i for i in new_instructions]
                break

    print(acc, ended)


if __name__ == "__main__":
    main()
