# https://www.codewars.com/kata/52d1bd3694d26f8d6e0000d3/train/python
# does not work with katakana alphabet
# can't really figure out why

class VigenereCipher(object):
    def __init__(self, key, alphabet):
        self._key = key
        self._alphabet = alphabet

        self._start = min(ord(o) for o in alphabet)
        self._end = max(ord(o) for o in alphabet)

    def _wrapOrd(self, o):
        while o > self._end:
            o -= self._end - self._start + 1
        while o < self._start:
            o += self._end - self._start + 1
        return o

    def _getKeyChar(self, i):
        return self._key[i % len(self._key)]

    def _convert(self, text, encode=True):
        out = []
        for i, c in enumerate(text):
            if c not in self._alphabet:
                out.append(c)
                continue
            shift = ord(self._getKeyChar(i)) - self._start

            if encode:
                code = self._wrapOrd(ord(c) + shift)
            else:
                # decoding
                code = self._wrapOrd(ord(c) - shift)

            out.append(chr(code))

        return "".join(out)

    def encode(self, text):
        return self._convert(text, encode=True)

    def decode(self, text):
        return self._convert(text, encode=False)
