# https://www.codewars.com/kata/540d0fdd3b6532e5c3000b5b/train/python
from string import ascii_letters
from math import factorial


# a*bx^n
class Monomial:
    def __init__(self, expression="") -> None:
        self._a = 0
        self._b = 0
        self._n = 1

        k = "".join(c for c in expression if c not in ascii_letters)
        x = "".join(c for c in expression if c in ascii_letters)

        if k == "-":
            self._a = -1
        elif k == "+":
            self._a = 1
        else:
            self._a = int(k) if k else 1
        self._b = x if x else ""

    def __str__(self) -> str:
        if self._n == 0:
            return "+1"

        if self._a == 0:
            return ""

        if self._a not in [-1, 1] or not self._b:
            if self._a > 0:
                a = "+" + str(self._a)
            else:
                a = str(self._a)
        else:
            if self._a > 0:
                a = "+"
            else:
                a = "-"

        if self._b:
            b = str(self._b)
        else:
            b = ""

        if self._n > 1 and self._b:
            n = "^" + str(self._n)
        else:
            n = ""

        return a + b + n

    def __mul__(self, other):
        if isinstance(other, int):
            self._a *= other
            return self

        if isinstance(other, Monomial):
            m = Monomial()

            if other._b == self._b:
                m._n = self._n + other._n
                m._a = self._a * other._a
                return m

            if not other._b and self._b:
                m._b = self._b
                m._n = self._n
                m._a = self._a * other._a
                return m

            if not self._b and other._b:
                m._b = other._b
                m._n = other._n
                m._a = self._a * other._a
                return m

            if not self._b and not other._b:
                m._n = 1
                m._b = ""
                m._a = self._a * other._a
                return m

    def __pow__(self, n):
        m = Monomial()

        if n == 0:
            m._n = 1
            m._a = 1
            m._b = ""
        else:
            m._n = self._n * n
            m._a = self._a ** n
            m._b = self._b

        return m


class Binomial:
    def __init__(self, expression: str) -> None:
        parts = expression.split("^")

        self._n = int(parts[1])
        self._monomials = []

        for i, c in enumerate(parts[0]):
            if c in ["+", "-"] and i > 1:
                self._monomials = [Monomial(x) for x in (
                    parts[0][1:i], parts[0][i:-1])]

    def _bin(self, n: int, k: int) -> int:
        return int(factorial(n) / (factorial(n-k) * factorial(k)))

    def expand(self):
        if self._n == 0:
            return "1"

        expanded = "".join(str(self._monomials[0] ** (self._n-k) *
                               self._monomials[1] ** k * self._bin(self._n, k))
                           for k in range(self._n + 1))

        if expanded[0] == "+":
            return expanded[1:]

        return expanded


def expand(expression) -> str:
    return Binomial(expression).expand()
