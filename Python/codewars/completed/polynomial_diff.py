# https://www.codewars.com/kata/566584e3309db1b17d000027/train/python

# ax ^ b


class Monomial:
    def __init__(self, string: str) -> None:
        self._a = 0
        self._b = 0
        self._valid = True

        parts = string.split("x")
        if len(parts) == 1 and parts[0] == "":
            self._valid = False
            return

        if not parts[0]:
            self._a = 1
        elif parts[0] == "-":
            self._a = -1
        elif parts[0] == "+":
            self._a = 1
        else:
            self._a = int(parts[0])

        if len(parts) == 2:
            self._b = int(parts[1][1:]) if parts[1] != "" else 1
        else:
            self._b = 0

    @property
    def valid(self):
        return self._valid

    def diff(self) -> None:
        self._a *= self._b
        self._b -= 1
        return self

    def evaluate(self, point: int) -> int:
        return int(self._a * (point ** self._b))


class Polynomial:
    def __init__(self, equation: str) -> None:
        self._monomials = []
        new_monomials = []
        start = 0

        for i, c in enumerate(e for e in equation):
            if c in ["+", "-"]:
                new_monomials.append(Monomial(equation[start:i]))
                start = i

        new_monomials.append(Monomial(equation[start:]))

        self._monomials = [m for m in new_monomials if m.valid]

    def diff(self):
        for m in self._monomials:
            m.diff()
        return self

    def evaluate(self, point: int) -> int:
        return sum(m.evaluate(point) for m in self._monomials)


def differentiate(equation: str, point: int) -> int:
    p = Polynomial(equation)
    return p.diff().evaluate(point)


def main():
    print(differentiate("12x+2", 3), 12)
    print(differentiate("x^2-x", 3), 5)
    print(differentiate("-5x^2+10x+4", 3), -20)


if __name__ == "__main__":
    main()
