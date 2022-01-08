# https://www.codewars.com/kata/57cf3dad05c186ba22000348/train/python
def format_unit(values):
    val = (values[0] * 10 + values[1]) * 10 ** values[2]

    for k, v in {1e6: "M", 1e3: "k"}.items():
        if k <= val:
            if (val / k).is_integer():
                return int(val / k), v
            return round(val / k, 1), v

    return val, ""


def decode_resistor_colors(bands):
    colours = {"black": 0, "brown": 1, "red": 2, "orange": 3, "yellow": 4,
               "green": 5, "blue": 6, "violet": 7, "gray": 8, "white": 9,
               "gold": "5%", "silver": "10%"}

    values = [colours[w] for w in bands.split(" ")]
    if len(values) == 3:
        values.append("20%")

    r, u = format_unit(values)
    return f"{r}{u} ohms, {values[3]}"
