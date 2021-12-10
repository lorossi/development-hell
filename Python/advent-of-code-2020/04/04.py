def load_passports(path):
    with open(path, "r") as f:
        raw = f.readlines()

    passports = []
    new_passport = {}

    for x in raw:
        words = x.strip().split(" ")

        if words[0] == "":
            passports.append(new_passport)
            new_passport = {}
        else:
            for word in words:
                field, value = word.split(":")
                new_passport[field] = value

    passports.append(new_passport)

    return passports


def validate_fields(passport):
    fields = [
        "byr",
        "iyr",
        "eyr",
        "hgt",
        "hcl",
        "ecl",
        "pid",
    ]
    return all(f in passport for f in fields)


def validate_dates(passport):
    if int(passport["byr"]) < 1920 or int(passport["byr"]) > 2002:
        return False

    if int(passport["iyr"]) < 2010 or int(passport["iyr"]) > 2020:
        return False

    if int(passport["eyr"]) < 2020 or int(passport["eyr"]) > 2030:
        return False

    return True


def validate_height(passport):
    if "cm" in passport["hgt"]:
        height = int(passport["hgt"][:-2])
        return 150 <= height <= 193

    if "in" in passport["hgt"]:
        height = int(passport["hgt"][:-2])
        return 59 <= height <= 76

    return False


def validate_hair_color(passport):
    if len(passport["hcl"]) != 7 and passport["hcl"][0] != "#":
        return False

    try:
        _ = int(passport["hcl"][1:], base=16)
        return True
    except ValueError:
        return False


def validate_eye_color(passport):
    colors = [
        "amb",
        "blu",
        "brn",
        "gry",
        "grn",
        "hzl",
        "oth",
    ]

    return any(c in passport["ecl"] for c in colors)


def validate_id(passport):
    if len(passport["pid"]) != 9:
        return False

    try:
        _ = int(passport["pid"])
        return True
    except ValueError:
        return False


def validate_passports(passports):
    valid = 0

    for p in passports:
        if not validate_fields(p):
            continue

        if not validate_dates(p):
            continue

        if not validate_height(p):
            continue

        if not validate_hair_color(p):
            continue

        if not validate_eye_color(p):
            continue

        if not validate_id(p):
            continue

        valid += 1

    return valid


def main():
    passports = load_passports("passports")
    valid = validate_passports(passports)
    print(valid)


if __name__ == "__main__":
    main()
