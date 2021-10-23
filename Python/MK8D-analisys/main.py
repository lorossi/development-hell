import csv
from glob import glob
from typing import List
from itertools import product


def load_rows(table: str, discard_header: bool = True) -> List:
    # sourcery skip: assign-if-exp
    with open(table, "r") as f:
        reader = csv.reader(f)

        if discard_header:
            rows = list(reader)[1:]
        else:
            rows = list(reader)

    return rows


def load_headers(table: str) -> List:
    with open(table, "r") as f:
        reader = csv.reader(f)

        for row in reader:
            return row


def convert_row(row: List) -> List:
    int_row = []

    for i in row:
        try:
            int_row.append(int(i))
        except (TypeError, ValueError):
            int_row.append(i)

    return int_row


def sum_rows(rows: List) -> List:  # sourcery skip: assign-if-exp
    int_rows = [convert_row(r) for r in rows]
    tuple_len = min(len(t) for t in int_rows)
    result = [None for _ in range(tuple_len)]

    for row_tuple in int_rows:
        for j, t in enumerate(row_tuple):
            if result[j]:
                if type(result[j]) == str:
                    result[j] += f"+{t}"
                else:
                    result[j] += t

            else:
                result[j] = t

    return result


def save_file(path: str, rows: list) -> None:  # sourcery skip: assign-if-exp
    with open(path, "w") as f:
        for row in rows:
            new_row = ""

            for item in row:
                if type(item) == str:
                    new_row += f"\"{item}\""
                else:
                    new_row += str(item)

                new_row += ","

            f.write(f"{new_row[:-1]}\n")


def cross_product(rows: List) -> List:
    result = [r for r in product(*rows)]
    return [sum_rows(r) for r in result]


def main():
    rows = [load_rows(file) for file in glob("datasets/*.csv")]
    result = cross_product(rows)
    headers = load_headers("datasets/2-karts.csv")
    result.insert(0, headers)
    save_file("result.csv", result)


if __name__ == "__main__":
    main()
