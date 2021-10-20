def main():
    with open("quotes-raw", "r") as f:
        raw_quotes = f.read().splitlines()

    quotes = []
    for line in raw_quotes:
        if not line:
            continue

        line = ". ".join(line.split(". ")[1:])  # remove number

        while "  " in line:
            line = line.replace("  ", " ")

        line = line.replace("\"", "'")
        line = line.replace(" –", "@~")
        line = line.strip()

        quotes.append(line)

    with open(".QUOTES", "w") as f:
        for line in quotes:
            f.write(f"{line}\n")


if __name__ == "__main__":
    main()
