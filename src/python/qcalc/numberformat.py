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

def to_decimal(number, decimal_spaces=32):
    return ("{:." + str(decimal_spaces) + "f}").format(number)


# Strip zeroes from decimal number because the str builtin either returns scientific notation or fixed decimal spaces,
# and the rstrip documentation could basically mean anything.
def remove_decimal_spaces(s):
    size = len(s)
    encountered_dot = False
    ret = ""
    i = 0
    while i < size:
        if s[i] == ".":
            has_decimal = False
            x = i + 1
            while x < size:
                if s[x] != "0":
                    has_decimal = True
                    break
                x += 1
            if not has_decimal:
                break
            encountered_dot = True
            ret = ret + s[i]
        elif encountered_dot:
            if s[i] == "0":
                x = i + 1
                last_zero = True
                while x < size:
                    if s[x] != "0":
                        last_zero = False
                        break
                    x += 1
                if last_zero:
                    break
                else:
                    ret = ret + s[i]
            else:
                ret = ret + s[i]
        else:
            ret = ret + s[i]
        i += 1
    return ret
