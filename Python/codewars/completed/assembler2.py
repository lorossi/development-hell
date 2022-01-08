"""
https://www.codewars.com/kata/58e61f3d8ff24f774400002c/train/python
"""

import unittest


def remove_spaces(text):
    while "  " in text:
        text = text.replace("  ", " ")
    return text


def parse_msg(text):
    msg = []

    text = text.replace("msg", "").strip()

    x = 0
    start = 0
    apex = False

    while x < len(text):
        if text[x] == "'":
            if not apex:
                apex = True
            else:
                msg.append(text[start:x])
                apex = False

            start = x+1
        elif text[x] == "," and not apex:
            msg.append(text[start:x])
            start = x+1

        x += 1

    msg.append(text[start:])

    return [m for m in msg if m]


def decode_instruction(instruction):
    clean = remove_spaces(instruction.strip().split(";")[0])

    if "msg" in clean:
        clean = clean.replace("msg ", "")
        words = ["msg", parse_msg(clean), 0]  # TODO
    elif ":" in clean:  # label
        words = ["label", clean.replace(":", ""), 0]
    else:
        words = clean.replace(", ", " ").split(" ")

        if len(words) < 3:
            for _ in range(len(words), 3):
                words.append(0)
        elif len(words) > 3:
            words = words[:3]

    return words


def assembler_interpreter(program):
    registers = {}
    instructions = {
        i: decode_instruction(p) for i, p in enumerate(x for x in program if x)
    }

    labels = {}
    for i, line in instructions.items():
        if line[0] == "label":
            labels[line[1]] = i

    stack_pointer = []
    w = 0
    msg = -1
    pc = 0
    max_pc = len(instructions)

    while pc < max_pc:
        op, a, b = instructions[pc]

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
                pc += int(b) - 1
        elif op == "add":
            try:
                registers[a] += int(b)
            except ValueError:
                registers[a] += registers[b]
        elif op == "sub":
            try:
                registers[a] -= int(b)
            except ValueError:
                registers[a] -= registers[b]
        elif op == "mul":
            try:
                registers[a] *= int(b)
            except ValueError:
                registers[a] *= registers[b]
        elif op == "div":
            try:
                registers[a] //= int(b)
            except ValueError:
                registers[a] //= registers[b]
        elif op == "jmp":
            pc = labels[a] - 1
        elif op == "cmp":
            try:
                wa = int(a)
            except ValueError:
                wa = registers[a]

            try:
                wb = int(b)
            except ValueError:
                wb = registers[b]

            w = wa - wb
        elif op == "jne" and w != 0:
            pc = labels[a] - 1
        elif op == "je" and w == 0:
            pc = labels[a] - 1
        elif op == "jge" and w >= 0:
            pc = labels[a] - 1
        elif op == "jg" and w > 0:
            pc = labels[a] - 1
        elif op == "jle" and w <= 0:
            pc = labels[a] - 1
        elif op == "jl" and w < 0:
            pc = labels[a] - 1
        elif op == "call":
            stack_pointer.append(pc)
            pc = labels[a] - 1
        elif op == "ret":
            pc = stack_pointer.pop()
        elif op == "msg":
            msg = ""

            for x in a:
                if x.strip() in registers:
                    msg += str(registers[x.strip()])
                else:
                    msg += x

        elif op == "end":
            return msg

        pc += 1

    return -1


class TestAssembler(unittest.TestCase):
    def test_cases(self):
        code = ['', '; My first program', 'mov  a, 5', 'inc  a',
                'call function', "msg  '(5+1)/2 = ', a    ; output message",
                'end', '', 'function:', '    div  a, 2', '    ret']

        solution = '(5+1)/2 = 3'
        self.assertEqual(assembler_interpreter(code), solution)

        code = ['', 'mov   a, 5', 'mov   b, a', 'mov   c, a',
                'call  proc_fact', 'call  print', 'end', '', 'proc_fact:',
                '    dec   b',
                '    mul   c, b', '    cmp   b, 1', '    jne   proc_fact',
                '    ret', '', 'print:',
                "    msg   a, '! = ', c ; output text", '    ret']

        solution = '5! = 120'
        self.assertEqual(assembler_interpreter(code), solution)

        code = ['', 'mov   a, 8            ; value',
                'mov   b, 0            ; next',
                'mov   c, 0            ; counter',
                'mov   d, 0            ; first',
                'mov   e, 1            ; second', 'call  proc_fib',
                'call  print', 'end', '', 'proc_fib:', '    cmp   c, 2',
                '    jl    func_0', '    mov   b, d',
                '    add   b, e', '    mov   d, e', '    mov   e, b',
                '    inc   c', '    cmp   c, a', '    jle   proc_fib',
                '    ret', '', 'func_0:', '    mov   b, c', '    inc   c',
                '    jmp   proc_fib', '', 'print:',
                "    msg   'Term ', a, ' of Fibonacci series is: ', \
                     b ; output text",
                '    ret']
        solution = 'Term 8 of Fibonacci series is: 21'
        self.assertEqual(assembler_interpreter(code), solution)

        code = ['', 'mov   a, 11           ; value1', 'mov   b, 3 ; value2',
                'call  mod_func',
                "msg   'mod(', a, ', ', b, ') = ', d ; output",
                'end', '', '; Mod function', 'mod_func:',
                '    mov   c, a        ; temp1', '    div   c, b',
                '    mul   c, b', '    mov   d, a        ; temp2',
                '    sub   d, c', '    ret']
        solution = "mod(11, 3) = 2"
        self.assertEqual(assembler_interpreter(code), solution)

        code = ['', 'mov   a, 81         ; value1',
                'mov   b, 153        ; value2', 'call  init',
                'call  proc_gcd', 'call  print', 'end', '', 'proc_gcd:',
                '    cmp   c, d', '    jne   loop', '    ret', '', 'loop:',
                '    cmp   c, d', '    jg    a_bigger', '    jmp   b_bigger',
                '', 'a_bigger:', '    sub   c, d', '    jmp   proc_gcd', '',
                'b_bigger:', '    sub   d, c',
                '    jmp   proc_gcd', '', 'init:', '    cmp   a, 0',
                '    jl    a_abs', '    cmp   b, 0', '    jl    b_abs',
                '    mov   c, a            ; temp1',
                '    mov   d, b            ; temp2', '    ret', '', 'a_abs:',
                '    mul   a, -1', '    jmp   init', '', 'b_abs:',
                '    mul   b, -1', '    jmp   init', '', 'print:',
                "    msg   'gcd(', a, ', ', b, ') = ', c", '    ret']
        solution = 'gcd(81, 153) = 9'
        self.assertEqual(assembler_interpreter(code), solution)

        code = ['', 'call  func1', 'call  print', 'end', '', 'func1:',
                '    call  func2', '    ret', '', 'func2:',
                '    ret', '', 'print:',
                "    msg 'This program should return -1'"]
        solution = -1
        self.assertEqual(assembler_interpreter(code), solution)

        code = ['', 'mov   a, 2            ; value1',
                'mov   b, 10           ; value2',
                'mov   c, a            ; temp1',
                'mov   d, b            ; temp2', 'call  proc_func',
                'call  print', 'end', '', 'proc_func:',
                '    cmp   d, 1', '    je    continue', '    mul   c, a',
                '    dec   d', '    call  proc_func', '', 'continue:',
                '    ret', '', 'print:', "    msg a, '^', b, ' = ', c",
                '    ret']
        self.assertEqual(assembler_interpreter(code), '2^10 = 1024')


def main():
    unittest.main()


if __name__ == "__main__":
    main()
