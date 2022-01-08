"""
https://www.codewars.com/kata/58e24788e24ddee28e000053/train/python
rn it works but it's way too slow
"""

import unittest


def decode_instructions(instruction):
    op, src, *dest = instruction.split(" ")
    if len(dest) == 0:
        dest = [0]

    return op, src, dest[0]


def simple_assembler(program):
    registers = {}
    instructions = {
        i: decode_instructions(p) for i, p in enumerate(program)
    }

    counter = 0
    max_counter = len(program)

    while counter < max_counter:
        op, a, b = instructions[counter]

        if op == "mov":
            try:
                registers[a] = int(b)
            except ValueError:
                registers[a] = registers[b]
        elif op == "inc":
            registers[a] += 1
        elif op == "dec":
            registers[a] -= 1
        elif op == "jnz":
            # WHAT THE FUCK THIS DOES NOT MAKE ANY SENSE
            # the code jumps even if the register is not defined
            # WHAT THE FUCK
            if registers.get(a, 1) != 0:
                counter += int(b) - 1

        counter += 1

    return registers


class TestAssembler(unittest.TestCase):
    def test_cases(self):
        code = ['mov a 5', 'inc a', 'dec a', 'dec a', 'jnz a -1', 'inc a']
        solution = {"a": 1}
        self.assertDictEqual(simple_assembler(code), solution)

        code = ['mov c 12', 'mov b 0', 'mov a 200', 'dec a', 'inc b',
                'jnz a -2', 'dec c', 'mov a b', 'jnz c -5', 'jnz 0 1',
                'mov c a']
        solution = {'a': 409600, 'c': 409600, 'b': 409600}

        self.assertDictEqual(simple_assembler(code), solution)

    def test_solutions(self):
        code = ['mov a 1', 'mov b 1', 'mov c 0', 'mov d 26', 'jnz c 2',
                'jnz 1 5', 'mov c 7', 'inc d', 'dec c', 'jnz c -2',
                'mov c a', 'inc a',  'dec b', 'jnz b -2', 'mov b c', 'dec d',
                'jnz d -6', 'mov c 18', 'mov d 11', 'inc a', 'dec d',
                'jnz d -2', 'dec c', 'jnz c -5']
        solution = {'a': 318009, 'b': 196418, 'c': 0, 'd': 0}
        self.assertDictEqual(simple_assembler(code), solution)

        code = ['mov d 100', 'dec d', 'mov b d', 'jnz b -2',
                'inc d', 'mov a d', 'jnz 5 10', 'mov c a']
        solution = {'a': 1, 'b': 0, 'd': 1}
        self.assertDictEqual(simple_assembler(code), solution)


def main():
    unittest.main()


if __name__ == "__main__":
    main()
