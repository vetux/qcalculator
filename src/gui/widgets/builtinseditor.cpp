/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "gui/widgets/builtinseditor.hpp"
#include "gui/widgets/scriptseditor.hpp"

#include <QVBoxLayout>
#include <QLabel>

BuiltInsEditor::BuiltInsEditor(QWidget *parent) : QWidget(parent) {
    setLayout(new QVBoxLayout());
    list = new QListWidget(this);
    list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    checkBox = new QCheckBox(this);
    checkBox->setText("Enable built-in constants (pi, epsilon and inf)");
    layout()->addWidget(checkBox);
    layout()->addWidget(list);
    connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));

    //Adapted from https://github.com/ArashPartow/exprtk/blob/806c519c91fd08ba4fa19380dbf3f6e42de9e2d1/readme.txt#L161
    setListContents({
                            {"+",         "Addition between x and y. (eg: x + y)"},
                            {"-",         "Subtraction between x and y. (eg: x - y)"},
                            {"*",         "Multiplication between x and y. (eg: x * y)"},
                            {"/",         "Division between x and y. (eg: x / y)"},
                            {"%",         "Modulus of x with respect to y. (eg: x % y)"},
                            {"^",         "x to the power of y. (eg: x ^ y)"},
                            {":=",        "Assign the value of x to y. Where y is either a variable or vector type. (eg: y := x)"},
                            {"+=",        "Increment x by the value of the expression on the right | hand side. Where x is either a variable or vector type. | (eg: x += abs(y - z))"},
                            {"-=",        "Decrement x by the value of the expression on the right | hand side. Where x is either a variable or vector type. | (eg: x[i] -= abs(y + z))"},
                            {"*=",        "Assign the multiplication of x by the value of the| expression on the righthand side to x. Where x is either| a variable or vector type.(eg: x *= abs(y / z))"},
                            {"/=",        "Assign the division of x by the value of the expression | on the right-hand side to x. Where x is either a variable or vector type. (eg: x[i + j] /= abs(y * z))"},
                            {"%=",        "Assign x modulo the value of the expression on the right| hand side to x. Where x is either a variable or vector | type. (eg: x[2] %= y ^ 2)"},
                            {"== or =",   "True only if x is strictly equal to y. (eg: x == y)"},
                            {"<> or !=",  "True only if x does not equal y. (eg: x <> y or x != y)"},
                            {"<",         "True only if x is less than y. (eg: x < y)"},
                            {"<=",        "True only if x is less than or equal to y. (eg: x <= y)"},
                            {">",         "True only if x is greater than y. (eg: x > y)"},
                            {">=",        "True only if x greater than or equal to y. (eg: x >= y)"},
                            {"true",      "True state or any value other than zero (typically 1)."},
                            {"false",     "False state, value of exactly zero."},
                            {"and",       "Logical AND, True only if x and y are both true.(eg: x and y)"},
                            {"mand",      "Multi - input logical AND, True only if all inputs are true. Left to right short-circuiting of expressions. (eg: mand(x > y, z < w, u or v, w and x))"},
                            {"mor",       "Multi-input logical OR, True if at least one of the inputs are true. Left to right short-circuiting of expressions. (eg: mor(x > y, z < w, u or v, w and x))"},
                            {"nand",      "Logical NAND, True only if either x or y is false. (eg: x nand y)"},
                            {"nor",       "Logical NOR, True only if the result of x or y is false (eg: x nor y)"},
                            {"not",       "Logical NOT, Negate the logical sense of the input. (eg: not(x and y) == x nand y)"},
                            {"or",        "Logical OR, True if either x or y is true. (eg: x or y)"},
                            {"xor",       "Logical XOR, True only if the logical states of x and y differ. (eg: x xor y)"},
                            {"xnor",      "Logical XNOR, True iff the biconditional of x and y is satisfied. (eg: x xnor y)"},
                            {"&",         "Similar to AND but with left to right expression short circuiting optimisation. (eg: (x & y) == (y and x))"},
                            {"|",         "Similar to OR but with left to right expression short circuiting optimisation. (eg: (x | y) == (y or x))"},
                            {"abs",       "Absolute value of x. (eg: abs(x))"},
                            {"avg",       "Average of all the inputs. (eg: avg(x,y,z,w,u,v) == (x + y + z + w + u + v) / 6)"},
                            {"ceil",      "Smallest integer that is greater than or equal to x."},
                            {"clamp",     "Clamp x in range between r0 and r1, where r0 < r1. (eg: clamp(r0,x,r1))"},
                            {"equal",     "Equality test between x and y using normalised epsilon"},
                            {"erf",       "Error function of x. (eg: erf(x))"},
                            {"erfc",      "Complimentary error function of x. (eg: erfc(x))"},
                            {"exp",       "e to the power of x. (eg: exp(x))"},
                            {"expm1",     "e to the power of x minus 1, where x is very small. (eg: expm1(x))"},
                            {"floor",     "Largest integer that is less than or equal to x."},
                            {"frac",      "Fractional portion of x. (eg: frac(x))"},
                            {"hypot",     "Hypotenuse of x and y (eg: hypot(x,y) = sqrt(x*x + y*y))"},
                            {"iclamp",    "Inverse-clamp x outside of the range r0 and r1. Where r0 < r1. If x is within the range it will snap to the closest bound. (eg: iclamp(r0,x,r1))"},
                            {"inrange",   "In-range returns 'true' when x is within the range r0 and r1. Where r0 < r1. (eg: inrange(r0,x,r1))"},
                            {"log",       "Natural logarithm of x. (eg: log(x))"},
                            {"log10",     "Base 10 logarithm of x. (eg: log10(x))"},
                            {"log1p",     "Natural logarithm of 1 + x, where x is very small. (eg: log1p(x))"},
                            {"log2",      "Base 2 logarithm of x. (eg: log2(x))"},
                            {"logn",      "Base N logarithm of x. where n is a positive integer. (eg: logn(x,8))"},
                            {"max",       "Largest value of all the inputs. (eg: max(x,y,z,w,u,v))"},
                            {"min",       "Smallest value of all the inputs. (eg: min(x,y,z,w,u))"},
                            {"mul",       "Product of all the inputs. (eg: mul(x,y,z,w,u,v,t) == (x * y * z * w * u * v * t))"},
                            {"ncdf",      "Normal cumulative distribution function. (eg: ncdf(x))"},
                            {"not_equal", "Not-equal test between x and y using normalised epsilon"},
                            {"pow",       "x to the power of y. (eg: pow(x,y) == x ^ y)"},
                            {"root",      "Nth-Root of x. where n is a positive integer. (eg: root(x,3) == x^(1/3))"},
                            {"round",     "Round x to the nearest integer. (eg: round(x))"},
                            {"roundn",    "Round x to n decimal places (eg: roundn(x,3)) where n > 0 and is an integer. (eg: roundn(1.2345678,4) == 1.2346)"},
                            {"sgn",       "Sign of x, -1 where x < 0, +1 where x > 0, else zero. (eg: sgn(x))"},
                            {"sqrt",      "Square root of x, where x >= 0. (eg: sqrt(x))"},
                            {"sum",       "Sum of all the inputs. (eg: sum(x,y,z,w,u,v,t) == (x + y + z + w + u + v + t))"},
                            {"swap",      "Swap the values of the variables x and y and return the"},
                            {"<=>",       "current value of y. (eg: swap(x,y) or x <=> y)"},
                            {"trunc",     "Integer portion of x. (eg: trunc(x))"},
                            {"acos",      "Arc cosine of x expressed in radians. Interval [-1,+1] (eg: acos(x))"},
                            {"acosh",     "Inverse hyperbolic cosine of x expressed in radians. (eg: acosh(x))"},
                            {"asin",      "Arc sine of x expressed in radians. Interval [-1,+1] (eg: asin(x))"},
                            {"asinh",     "Inverse hyperbolic sine of x expressed in radians. (eg: asinh(x))"},
                            {"atan",      "Arc tangent of x expressed in radians. Interval [-1,+1] (eg: atan(x))"},
                            {"atan2",     "Arc tangent of (x / y) expressed in radians. [-pi,+pi] eg: atan2(x,y)"},
                            {"atanh",     "Inverse hyperbolic tangent of x expressed in radians. (eg: atanh(x))"},
                            {"cos",       "Cosine of x. (eg: cos(x))"},
                            {"cosh",      "Hyperbolic cosine of x. (eg: cosh(x))"},
                            {"cot",       "Cotangent of x. (eg: cot(x))"},
                            {"csc",       "Cosecant of x. (eg: csc(x))"},
                            {"sec",       "Secant of x. (eg: sec(x))"},
                            {"sin",       "Sine of x. (eg: sin(x))"},
                            {"sinc",      "Sine cardinal of x. (eg: sinc(x))"},
                            {"sinh",      "Hyperbolic sine of x. (eg: sinh(x))"},
                            {"tan",       "Tangent of x. (eg: tan(x))"},
                            {"tanh",      "Hyperbolic tangent of x. (eg: tanh(x))"},
                            {"deg2rad",   "Convert x from degrees to radians. (eg: deg2rad(x))"},
                            {"deg2grad",  "Convert x from degrees to gradians. (eg: deg2grad(x))"},
                            {"rad2deg",   "Convert x from radians to degrees. (eg: rad2deg(x))"},
                            {"grad2deg",  "Convert x from gradians to degrees. (eg: grad2deg(x))"},
                            {"in",        "True only if x is a substring of y.(eg: x in y or 'abc' in 'abcdefgh')"},
                            {"like",      "True only if the string x matches the pattern y. Available wildcard characters are '*' and '?' denoting zero or more and zero or one matches respectively. (eg: x like y or 'abcdefgh' like 'a?d*h')"},
                            {"ilike",     "True only if the string x matches the pattern y in a case insensitive manner. Available wildcard characters are '*' and '?' denoting zero or more and zero or one matches respectively. (eg: x ilike y or 'a1B2c3D4e5F6g7H' ilike 'a?d*h')"},
                            {"[r0:r1]",   "The closed interval [r0,r1] of the specified string."},
                            {":=",        "Assign the value of x to y. Where y is a mutable string or string range and x is either a string or a string range."},
                            {"+",         "Concatenation of x and y. Where x and y are strings or string ranges."},
                            {"+=",        "Append to x the value of y. Where x is a mutable string and y is either a string or a string range."},
                            {"<=>",       "Swap the values of x and y. Where x and y are mutable strings. (eg: x <=> y)"},
                            {"[]",        "The string size operator returns the size of the string being actioned."},
                            {"if",        "If x is true then return y else return z."},
                            {"if-else",   "The if-else/else-if statement. Subject to the condition branch the statement will return either the value of the consequent or the alternative branch."},
                            {"switch",    "The first true case condition that is encountered will determine the result of the switch. If none of the case conditions hold true, the default action is assumed as the final return value. This is sometimes also known as a multi-way branch mechanism. eg: switch { case x > (y + z) : 2 * x / abs(y - z); case x < 3: sin(x + y); default: 1 + x; }"},
                            {"while",     "The structure will repeatedly evaluate the internalstatement(s) 'while' the condition is true. The final statement in the final iteration will be used as the return value of the loop. eg: while ((x -= 1) > 0) { y := x + z; w := u + y; }"},
                            {"repeat/until",    "The structure will repeatedly evaluate the internalstatement(s) 'until' the condition is true. The final statement in the final iteration will be used as the return value of the loop. eg: repeat y := x + z; w := u + y; until ((x += 1) > 100)"},
                            {"for",       "The structure will repeatedly evaluate the internalstatement(s) while the condition is true. On each loop iteration, an 'incrementing' expression is evaluated. The conditional is mandatory whereas the initialiser and incrementing expressions are optional. eg: for (var x := 0; (x < n) and (x != y); x += 1) {y := y + x / 2 - z;w := u + y; }"},
                            {"break/break[]",     "Break terminates the execution of the nearest enclosed loop, allowing for the execution to continue on external to the loop. The default break statement will set the return value of the loop to NaN, where as the return based form will set the value to that of the break expression. eg: while ((i += 1) < 10) {if (i < 5) j -= i + 2;else if (i % 2 == 0) break; else break[2i + 3]; }"},
                            {"continue",  "Continue results in the remaining portion of the nearest enclosing loop body to be skipped. eg: for (var i := 0; i < 10; i += 1) {if (i < 5) continue; j -= i + 2;}"},
                            {"return",    "Return immediately from within the current expression. With the option of passing back a variable number of values (scalar, vector or string). eg: 1. return [1]; 2. return [x, 'abx']; 3. return [x, x + y,'abx']; 4. return [];5. if (x < y) return [x, x - y, 'result-set1', 123.456];else return [y, x + y, 'result-set2'];"},
                            {"?:",        "Ternary conditional statement, similar to that of the above denoted if-statement. eg: 1. x ? y : z 2. x + 1 > 2y ? z + 1 : (w / v)3. min(x,y) > z ? (x < y + 1) ? x : y : (w * v)"},
                            {"~",         "Evaluate each sub-expression, then return as the result the value of the last sub-expression. This is sometimes known as multiple sequence point evaluation. eg: ~(i := x + 1, j := y / z, k := sin(w/u)) == (sin(w/u))) ~{i := x + 1; j := y / z; k := sin(w/u)} == (sin(w/u)))"},
                            {"[*]",       "Evaluate any consequent for which its case statement is true. The return value will be either zero or the result of the last consequent to have been evaluated. eg: [*] {case (x + 1) > (y - 2): x := z / 2 + sin(y / pi); case (x + 2) < abs(y + 3) : w / 4 + min(5y,9); case (x + 3) == (y * 4): y := abs(z / 6) + 7y;}"},
                            {"[]",        "The vector size operator returns the size of the vector being actioned. eg: 1. v[] 2. max_size := max(v0[],v1[],v2[],v3[])"}
                    });
}

void BuiltInsEditor::setUseBuiltInConstants(bool useBuiltIns) {
    checkBox->setCheckState(useBuiltIns ? Qt::Checked : Qt::Unchecked);
}

void BuiltInsEditor::setListContents(const std::vector<std::pair<QString, QString>> &contents) {
    list->clear();
    for (auto &pair: contents) {
        auto *widget = new QWidget(list);
        auto *item = new QListWidgetItem();

        auto *label = new QLabel(widget);
        auto *descLabel = new QLabel(widget);

        label->setMinimumWidth(100);
        label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        label->setText(pair.first);
        descLabel->setText(pair.second);

        auto *layout = new QHBoxLayout;
        layout->addWidget(label);
        layout->addWidget(descLabel, 1);

        widget->setLayout(layout);

        item->setSizeHint(widget->sizeHint());

        list->addItem(item);
        list->setItemWidget(item, widget);

        list->update();
    }
}

void BuiltInsEditor::stateChanged(int state) {
    emit onUseBuiltInConstantChanged(state == Qt::Checked);
}
