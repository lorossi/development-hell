import unittest
from table import TableHandler


class TestStringMethods(unittest.TestCase):
    def test_load(self):
        _ = TableHandler("test.csv")

    def test_headers(self):
        t = TableHandler("test.csv")
        self.assertListEqual(t.keys, ["name", "a", "b", "c", "d"])

    def test_table(self):
        t = TableHandler("test.csv")
        a = str(t.table)
        print(a)


if __name__ == '__main__':
    unittest.main()
