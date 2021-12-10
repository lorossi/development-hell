from functools import reduce


def load_map(path):
    with open(path, "r") as f:
        lines = f.readlines()

    return [x.strip() for x in lines]


def count_trees(tree_map, dx=3, dy=1):
    y = 0
    x = 0
    trees = 0

    while y < len(tree_map) - dy:
        x = (x + dx) % len(tree_map[0])
        y += dy
        if tree_map[y][x] == "#":
            trees += 1

    return trees


def main():
    tree_map = load_map("map")

    positions = [[1, 1], [3, 1], [5, 1], [7, 1], [1, 2]]

    trees = [
        count_trees(tree_map, p[0], p[1])
        for p in positions
    ]
    total = reduce(lambda x, y: x * y, trees)
    print(total)


if __name__ == "__main__":
    main()
