def load_passwords(path):
    with open(path, "r") as f:
        lines = f.readlines()

    passwords = []

    for x in lines:
        plain = x.replace("-", " ").replace(":", "").strip().split(" ")
        passwords.append({
            "min": int(plain[0]),
            "max": int(plain[1]),
            "letter": plain[2],
            "password": plain[3]
        })

    return passwords


def validate_passwords(passwords):
    return sum(
        p["min"] <= p["password"].count(p["letter"]) <= p["max"]
        for p in passwords
    )


def validate_passwords_two(passwords):
    valid = 0
    for p in passwords:
        found = sum(
            p["password"][x-1] == p["letter"]
            for x in [p["min"], p["max"]]
        )

        if found == 1:
            valid += 1

    return valid


def main():
    passwords = load_passwords("passwords")
    valid = validate_passwords(passwords)
    print(valid)
    valid = validate_passwords_two(passwords)
    print(valid)


if __name__ == "__main__":
    main()
