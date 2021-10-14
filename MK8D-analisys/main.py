from table import TableHandler


def main():
    t = TableHandler("test.csv")
    t2 = TableHandler("test2.csv")

    t3 = t.multiply(t2)

    print(t3)


if __name__ == "__main__":
    main()
