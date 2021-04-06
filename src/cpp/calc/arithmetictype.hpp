#ifndef QCALC_ARITHMETICTYPE_HPP
#define QCALC_ARITHMETICTYPE_HPP

/**
 * The fundamental type used for arithmetic.
 *
 * For now we will use double to match the cpython float implementation.
 *
 * To achieve perfect precision a arbitrary precision number format has to be implemented.
 *
 * This could then be exposed to python with a custom type,
 * and would also have to be integrated with exprtk.
*/
typedef double ArithmeticType;

#endif //QCALC_ARITHMETICTYPE_HPP
