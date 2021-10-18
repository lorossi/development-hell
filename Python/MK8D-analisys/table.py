class TableHandler:
    def __init__(self, path: str) -> None:
        self._path = path
        self._table = Table()

        self._load()

    def _load(self) -> None:
        with open(self._path, "r") as csv:
            lines = csv.readlines()

        for i, line in enumerate(lines):
            line = line.strip().replace("\n", "")
            fields = line.split(",")

            if i == 0:
                self._table.setKeys(fields)
            else:
                self._table.addLine(fields)

    def multiply(self, other):
        result = Table()

    @property
    def keys(self):
        return self._table.keys

    @property
    def table(self):
        return self._table


class Table:
    def __init__(self) -> None:
        self.clear()

    def clear(self):
        self._keys = []
        self._table = {}

    def setKeys(self, keys: list) -> None:
        self._keys = [k for k in keys]
        self._table = {k: [] for k in keys}
        self._table = {k: [] for k in self._keys}

    def addLine(self, fields: list) -> None:
        for i, f in enumerate(fields):
            key = self._keys[i]
            self._table[key].append(f)

    def __str__(self):  # sourcery skip: assign-if-exp
        keys = [k for k in self._keys]
        ret = "| " + " | ".join(keys) + " |\n"
        ret += "|---" * len(keys) + "|\n"

        for x in range(max(len(k) for _, k in self._table.items())):
            ret += "| "
            for k in self._table:
                if len(self._table[k]) > x:
                    ret += f"{self._table[k][x]} | "
                else:
                    ret += " | "

            ret += "\n"

        return ret

    @ property
    def keys(self):
        return [k for k in self._keys]
