from math import log10


def encode_resistor_colors(ohms_string):
    colours = {"0": "black", "1": "brown", "2": "red", "3": "orange",
               "4": "yellow", "5": "green", "6": "blue", "7": "violet",
               "8": "gray", "9": "white"}

    units = {
        "M": 1e6,
        "k": 1e3,
    }

    found = False
    for u, v in units.items():
        if u in ohms_string:
            val = float(ohms_string.replace(" ohms", "")[:-1]) * v
            found = True

    if not found:
        val = float(ohms_string.replace(" ohms", ""))

    out = []
    for x in range(2):
        out.append(colours[str(val)[x]])

    out.append(colours[str(int(log10(val)) - 1)])
    out.append("gold")

    return " ".join(out)
