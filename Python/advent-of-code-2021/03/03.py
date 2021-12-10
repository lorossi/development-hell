from math import log2, pow


def open_file(path="input"):
    with open(path, "r") as f:
        lines = f.readlines()

    return [int(x.strip(), 2) for x in lines]


def bit_frequency(diagnostic, bit):
    frequencies = [0, 0]
    for d in diagnostic:
        b = bit_mask(d, bit)
        frequencies[b] += 1

    return frequencies


def bit_mask(number, bit):
    return (number >> bit) & 1


def filter_diagnostic(diagnostic, bit, value):
    return [d for d in diagnostic if bit_mask(d, bit) == value]


def phase_1(diagnostic):
    bits = int(log2(max(diagnostic)) + 1)

    frequencies = [
        [0, 0] for _ in range(bits)
    ]

    for d in diagnostic:
        for i in range(bits):
            b = bit_mask(d, bits-i-1)
            frequencies[i][b] += 1

    gamma = 0
    epsilon = 0
    for f in frequencies:
        gamma <<= 1
        epsilon <<= 1

        if f[0] > f[1]:
            gamma += 0
            epsilon += 1
        else:
            gamma += 1
            epsilon += 0
    return gamma, epsilon


def phase_2(diagnostic):
    bits = int(log2(max(diagnostic)) + 1)

    oxy_list = diagnostic[:]

    bit_count = bits
    while len(oxy_list) > 1:
        freq = bit_frequency(oxy_list, bit_count)

        if freq[0] == freq[1]:
            oxy_list = filter_diagnostic(oxy_list, bit_count, 1)
        else:
            oxy_list = filter_diagnostic(
                oxy_list, bit_count, freq.index(max(freq)))

        bit_count -= 1

    co2_list = diagnostic[:]
    bit_count = bits
    while len(co2_list) > 1:
        freq = bit_frequency(co2_list, bit_count)

        if freq[0] == freq[1]:
            co2_list = filter_diagnostic(co2_list, bit_count, 0)
        else:
            co2_list = filter_diagnostic(
                co2_list, bit_count, freq.index(min(freq)))

        bit_count -= 1

    print(oxy_list, co2_list)
    return oxy_list[0], co2_list[0]


def main():
    diagnostic = open_file("test")
    g, e = phase_1(diagnostic)
    print(g * e)
    oxy, co2 = phase_2(diagnostic)
    print(oxy * co2)


if __name__ == "__main__":
    main()
