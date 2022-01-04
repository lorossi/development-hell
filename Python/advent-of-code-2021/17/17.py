def simulate(target, vx=0, vy=0):
    x = 0
    y = 0
    max_y = 0

    while x <= target["x"][1] and y >= target["y"][0]:
        x += vx
        y += vy

        if y > max_y:
            max_y = y

        if vx > 0:
            vx -= 1

        vy -= 1

        if target["x"][0] <= x <= target["x"][1] and \
                target["y"][0] <= y <= target["y"][1]:
            return max_y, True

    return -1, False


def question_one(target):
    max_y = 0
    for vx in range(0, target["x"][1] + 1):
        for vy in range(0, abs(target["y"][0]) + 1):
            new_max, inside = simulate(target, vx, vy)
            if inside and new_max > max_y:
                max_y = new_max

    return max_y


def question_two(target):
    count = 0
    for vx in range(0, target["x"][1] + 1):
        for vy in range(target["y"][0], abs(target["y"][0]) + 1):
            _, inside = simulate(target, vx, vy)
            if inside:
                count += 1

    return count


def main():
    target = {
        "x": [138, 184],
        "y": [-125, -71],
    }

    answer = question_one(target)
    print(answer)
    answer = question_two(target)
    print(answer)


if __name__ == "__main__":
    main()
