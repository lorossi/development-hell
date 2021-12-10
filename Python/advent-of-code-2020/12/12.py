from math import sin, cos, radians


def open_file(path="input"):
    with open(path, "r") as f:
        lines = f.readlines()

    return [x.strip() for x in lines]


def execute_instructions(instructions):
    wx, wy = 10, 1  # waypoint position
    sx, sy = 0, 0  # ship position

    wa = 0
    rotated = False

    for i in instructions:
        opcode = i[:1]
        value = int(i[1:])

        if opcode == "F":
            sx += wx * value
            sy += wy * value
        elif opcode == "N":
            wy += value
        elif opcode == "S":
            wy -= value
        elif opcode == "E":
            wx += value
        elif opcode == "W":
            wx -= value
        elif opcode == "L":
            wa = value
            rotated = True
        elif opcode == "R":
            wa = -value
            rotated = True

        if rotated:
            rotated = False
            theta = radians(wa)
            wxn = wx * cos(theta) - wy * sin(theta)
            wyn = wx * sin(theta) + wy * cos(theta)
            wx, wy = round(wxn), round(wyn)

    return abs(sx), abs(sy)


def main():
    instructions = open_file()
    x, y = execute_instructions(instructions)
    print(x, y, x + y)


if __name__ == "__main__":
    main()
