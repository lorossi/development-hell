def open_file(path="input"):
    with open(path, "r") as f:
        lines = f.readlines()

    return [x.strip() for x in lines]


def main():
    ...


if __name__ == "__main__":
    main()
