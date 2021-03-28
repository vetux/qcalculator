#ifndef QCALC_ARITHMETICTYPE_HPP
#define QCALC_ARITHMETICTYPE_HPP

/**
* The fundamental type used for arithmetic.
*
* More sophisticated implementation may use different fundamental types or implement custom number encodings to achieve strongest possible accuracy.
*
* With most long double implementations accuracy problems are generally only visible for very small fractions
* or very large numbers which could not be represented with language provided integer encodings.
*/
typedef long double ArithmeticType;

#endif //QCALC_ARITHMETICTYPE_HPP
