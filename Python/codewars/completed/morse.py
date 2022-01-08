# https://www.codewars.com/kata/54b72c16cd7f5154e9000457/train/python
MORSE_CODE = {
    '.-': 'A', '-...': 'B', '-.-.': 'C', '-..': 'D', '.': 'E', '..-.': 'F',
    '--.': 'G', '....': 'H', '..': 'I', '.---': 'J', '-.-': 'K', '.-..': 'L',
    '--': 'M', '-.': 'N', '---': 'O', '.--.': 'P', '--.-': 'Q', '.-.': 'R',
    '...': 'S', '-': 'T', '..-': 'U', '...-': 'V', '.--': 'W', '-..-': 'X',
    '-.--': 'Y', '--..': 'Z', '-----': '0', '.----': '1', '..---': '2',
    '...--': '3', '....-': '4', '.....': '5', '-....': '6', '--...': '7',
    '---..': '8', '----.': '9', '.-.-.-': '.', '--..--': ',', '..--..': '?',
    '.----.': "'", '-.-.--': '!', '-..-.': '/', '-.--.': '(', '-.--.-': ')',
    '.-...': '&', '---...': ':', '-.-.-.': ';', '-...-': '=', '.-.-.': '+',
    '-....-': '-', '..--.-': '_', '.-..-.': '"', '...-..-': '$',
    '.--.-.': '@', '...---...': 'SOS'
}


def shortest_sequence(string, char):
    words = [s for s in "".join(
        [s if s == char else "~" for s in string]).split("~") if s]
    if not words:
        return 0
    return min(len(x) for x in words)


def strip_zeroes(string):
    first = 0
    last = len(string)

    for x in range(len(string)):
        if string[x] == "0":
            first += 1
        else:
            break
    for x in range(len(string) - 1, -1, -1):
        if string[x] == "0":
            last -= 1
        else:
            break

    return string[first:last]


def decode_bits(bits):
    stripped = strip_zeroes(bits)
    shortest = [shortest_sequence(stripped, x) for x in ["0", "1"]]

    if shortest[0] > 0 and shortest[1] % 3 == 0 and shortest[1] % shortest[0] == 0:
        time_unit = min(shortest)
    else:
        time_unit = shortest[1]

    replace_map = {
        "111" * time_unit: "-",
        "1" * time_unit: ".",
        "000" * time_unit: " ",
        "0": "",
    }

    for k, v in replace_map.items():
        bits = bits.replace(k, v)

    return bits


def decode_morse(morse_code):
    return "".join(MORSE_CODE.get(x, " ") for x in morse_code.strip().replace("   ", " ~ ").split(" "))
