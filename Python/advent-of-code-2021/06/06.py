

def load_initial(path="input"):
    with open(path, "r") as f:
        line = [int(x) for x in f.readlines()[0].split(",")]

    fish = {x: 0 for x in range(0, 9)}
    for f in line:
        fish[f] += 1

    return fish


def simulate_day(fish):
    new = fish[0]

    for x in range(1, 9):
        fish[x-1] = fish[x]

    fish[6] += new
    fish[8] = new

    return fish


def main():
    days = 256
    fish = load_initial("input")

    for x in range(days):
        print(f"Day {x+1}")
        fish = simulate_day(fish)

    answer = sum(fish.values())
    print(answer)


if __name__ == "__main__":
    main()
