import _mpreal

mpreal = _mpreal.mpreal

class RoundingMode:
    def __init__(self):
        pass

    ROUND_NEAREST = 0
    ROUND_TOWARD_ZERO = 1
    ROUND_TOWARD_INFINITY = 2
    ROUND_TOWARD_INFINITY_NEGATIVE = 3
    ROUND_AWAY_FROM_ZERO = 4

def _get_rounding_char(mode):
    if mode == RoundingMode.ROUND_NEAREST:
        return "N"
    elif mode == RoundingMode.ROUND_TOWARD_ZERO:
        return "Z"
    elif mode == RoundingMode.ROUND_TOWARD_INFINITY:
        return "U"
    elif mode == RoundingMode.ROUND_TOWARD_INFINITY_NEGATIVE:
        return "D"
    elif mode == RoundingMode.ROUND_AWAY_FROM_ZERO:
        return "Y"


def to_decimal(number, precision=10):
    return number.to_string("%."
                            + str(precision)
                            + "R"
                            + str(_get_rounding_char(number.get_default_rounding()))
                            + "f").rstrip("0").rstrip(".")


def from_decimal(text):
    return mpreal(text, 10)


def to_hex(number, precision=10):
    if number.is_integer():
        # This expression invokes the modulus operator on the unicode object which in turn calls PyUnicode_Format()
        return ''.join('%x' % int(number))
    else:
        return number.to_string("%."
                                + str(precision)
                                + "R"
                                + str(_get_rounding_char(number.get_default_rounding()))
                                + "a")


def from_hex(text):
    return mpreal(text, 16)


def to_octal(number):
    return ''.join('%o' % int(number))


def from_octal(text):
    return mpreal(text, 8)


def to_binary(number):
    return "{0:b}".format(int(number))


def from_binary(text):
    return mpreal(text, 2)