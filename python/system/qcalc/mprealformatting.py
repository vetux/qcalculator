# QCalc - Extensible programming calculator
# Copyright (C) 2021  Julian Zampiccoli
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

from qcalc.exprtk import mpreal as mpreal
import qcalc.exprtk


def _get_rounding_char(mode):
    if mode == qcalc.exprtk.RoundingMode.ROUND_NEAREST:
        return "N"
    elif mode == qcalc.exprtk.RoundingMode.ROUND_TOWARD_ZERO:
        return "Z"
    elif mode == qcalc.exprtk.RoundingMode.ROUND_TOWARD_INFINITY:
        return "U"
    elif mode == qcalc.exprtk.RoundingMode.ROUND_TOWARD_INFINITY_NEGATIVE:
        return "D"
    elif mode == qcalc.exprtk.RoundingMode.ROUND_AWAY_FROM_ZERO:
        return "Y"


def to_decimal(number, precision=9):
    return number.to_string("%."
                     + str(precision)
                     + "R"
                     + str(_get_rounding_char(number.get_default_rounding()))
                     + "f").rstrip("0").rstrip(".")


def from_decimal(text):
    return mpreal(text, 10)


def to_hex(number, precision=9):
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
