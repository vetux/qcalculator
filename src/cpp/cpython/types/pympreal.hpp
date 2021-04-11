#ifndef QCALC_PYMPREAL_HPP
#define QCALC_PYMPREAL_HPP

namespace mpfr {
    class mpreal;
}

struct _object;
typedef _object PyObject;

bool PyMpReal_Initialize(PyObject *module);

PyObject *PyMpReal_FromMpReal(const mpfr::mpreal &val);

mpfr::mpreal PyMpReal_AsMpReal(PyObject *op);

bool PyMpReal_Check(PyObject *op);

#endif //QCALC_PYMPREAL_HPP
