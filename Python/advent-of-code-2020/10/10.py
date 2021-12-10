from functools import lru_cache


def load_joltages(path):
    with open(path, "r") as f:
        lines = f.readlines()

    return [int(x.strip()) for x in lines]


def find_chains(joltages):
    joltages.sort()
    chains_len = [1, 1]

    for i in range(len(joltages) - 1):
        if joltages[i+1] - joltages[i] == 1:
            chains_len[0] += 1
        elif joltages[i+1] - joltages[i] == 3:
            chains_len[1] += 1

    return chains_len[0], chains_len[1]


def create_graph(joltages):
    graph = {}
    joltages.sort()

    for j in joltages:
        for k in joltages:

            diff = k - j
            if diff <= 0 or diff > 3:
                continue

            if not graph.get(j):
                graph[j] = [k]
            else:
                graph[j].append(k)

    return graph


def find_all_paths(graph, start=0, end=0):

    if start == end:
        return 1

    return sum(find_all_paths(graph, start=n, end=end) for n in graph[start])


def main():
    joltages = load_joltages("joltages")
    # joltages = load_joltages("tests")

    # chain1, chain3 = find_chains(joltages)
    # print(chain1 * chain3)

    # for i, v in enumerate(valid_permutations):
    #     print(i + 1, v)

    # print(f"Total found: {len(valid_permutations)}")
    # print(f"It took {elapsed} seconds")

    graph = create_graph(joltages)
    end = max(max(v) for _, v in graph.items())

    for k, v in graph.items():
        print(k, v)

    paths = sum(find_all_paths(graph, end=end, start=j) for j in range(1, 4))

    print(paths)


if __name__ == "__main__":
    main()
