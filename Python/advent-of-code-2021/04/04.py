from copy import deepcopy


def open_file(path="input"):
    with open(path, "r") as f:
        lines = f.readlines()

    return [x.strip() for x in lines]


def parse_bingo(raw_bingo):
    extractions = [int(x) for x in raw_bingo[0].split(",")]

    boards = []
    id = 0
    for x in range(2, len(raw_bingo), 6):
        new_board = [None for _ in range(5)]
        for y in range(5):
            new_board[y] = [int(x) for x in raw_bingo[x+y].split(" ") if x]
        boards.append({
            "board": new_board,
            "won": False,
            "id": id + 1,
            "extraction": -1,
        })
        id += 1

    return extractions, boards


def check_row(board, y):
    return all(board[y][x] == -1 for x in range(5))


def check_col(board, x):
    return all(board[y][x] == -1 for y in range(5))


def check_board(board):
    for y in range(5):
        if check_row(board, y):
            return True

    for x in range(5):
        if check_col(board, x):
            return True

    return False


def mark_num(board, num):
    for y in range(5):
        for x in range(5):
            if board[y][x] == num:
                board[y][x] = -1
                return True

    return False


def sum_board(board):
    tot = 0
    for y in range(5):
        for x in range(5):
            if board[y][x] != -1:
                tot += board[y][x]

    return tot


def run_extractions_1(extractions, boards):
    for e in extractions:
        for b in boards:
            ret = mark_num(b, e)

            if ret and check_board(b):
                return e, sum_board(b)


def check_winners(extractions, boards):
    won = []
    for e in extractions:
        for b in boards:
            ret = mark_num(b["board"], e)

            if ret and not b["won"] and check_board(b["board"]):
                b["extraction"] = e
                b["won"] = True
                won.append(b["id"])

            if all(bb["won"] for bb in boards):
                return won


def run_extractions_2(extractions, boards):
    won = []
    copy_boards = deepcopy(boards)

    won = check_winners(extractions, copy_boards)

    for b in copy_boards:
        if b["id"] == won[-1]:
            return b["extraction"], sum_board(b["board"])


def main():
    raw_bingo = open_file("input")
    extractions, boards = parse_bingo(raw_bingo)
    # e, b = run_extractions_1(extractions, boards)
    # print(e, b, e * b)
    e, b = run_extractions_2(extractions, boards)
    print(e, b, e * b)


if __name__ == "__main__":
    main()
