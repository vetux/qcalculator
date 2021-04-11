#ifndef QCALC_PYMPREAL_HPP
#define QCALC_PYMPREAL_HPP

#include "Python.h"

#include "extern/mpreal.h"

typedef struct {
    PyObject_HEAD
    mpfr::mpreal mpreal;
} PyMpRealObject;

PyObject *PyMpReal_FromMpReal(double fval) {
    return PyNull;
}

double PyMpReal_AsMpReal(PyObject *op) {
    return 0;
}

PyObject *mpreal_richcompare(PyObject *v, PyObject *w, int op) {
    switch (op) {
        case Py_EQ:
            break;
        case Py_NE:
            break;
        case Py_LE:
            break;
        case Py_GE:
            break;
        case Py_LT:
            break;
        case Py_GT:
            break;
    }
    return PyNull;
}

Py_hash_t mpreal_hash(PyMpRealObject *v) {
    return {};
}

PyObject *mpreal_add(PyObject *v, PyObject *w) {
    return PyNull;
}

PyObject *mpreal_sub(PyObject *v, PyObject *w) {
    return PyNull;
}

PyObject *mpreal_mul(PyObject *v, PyObject *w) {
    return PyNull;
}

PyObject *mpreal_div(PyObject *v, PyObject *w) {
    return PyNull;
}

PyObject *mpreal_rem(PyObject *v, PyObject *w) {
    return PyNull;
}

PyObject *mpreal_divmod(PyObject *v, PyObject *w) {
    return PyNull;
}

PyObject *mpreal_floor_div(PyObject *v, PyObject *w) {
    return PyNull;
}

PyObject *mpreal_pow(PyObject *v, PyObject *w, PyObject *z) {
    return PyNull;
}

PyObject *mpreal_neg(PyMpRealObject *v) {
    return PyNull;
}

PyObject *mpreal_abs(PyMpRealObject *v) {
    return PyNull;
}

int mpreal_bool(PyMpRealObject *v) {
    return 0;
}

PyObject *mpreal_float(PyObject *v) {
    return PyNull;
}

PyObject *mpreal_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    return PyNull;
}

void mpreal_dealloc(PyMpRealObject *op) {
    Py_TYPE(op)->tp_free((PyObject *) op);
}

static PyMethodDef mpreal_methods[] = {
        {PyNull, PyNull}           /* sentinel */
};

static PyNumberMethods MpRealNumberMethods = {
        mpreal_add,                 /* nb_add */
        mpreal_sub,                 /* nb_subtract */
        mpreal_mul,                 /* nb_multiply */
        mpreal_rem,                 /* nb_remainder */
        mpreal_divmod,              /* nb_divmod */
        mpreal_pow,                 /* nb_power */
        (unaryfunc) mpreal_neg,     /* nb_negative */
        mpreal_float,               /* nb_positive */
        (unaryfunc) mpreal_abs,     /* nb_absolute */
        (inquiry) mpreal_bool,      /* nb_bool */
        PyNull,                 /* nb_invert */
        PyNull,                  /* nb_lshift */
        PyNull,                  /* nb_rshift */
        PyNull,                   /* nb_and */
        PyNull,                   /* nb_xor */
        PyNull,                  /* nb_or */
        PyNull,                  /* nb_int */
        PyNull,                  /* nb_reserved */
        mpreal_float,        /* nb_float */
        PyNull,                  /* nb_inplace_add */
        PyNull,                  /* nb_inplace_subtract */
        PyNull,                  /* nb_inplace_multiply */
        PyNull,                  /* nb_inplace_remainder */
        PyNull,                  /* nb_inplace_power */
        PyNull,                  /* nb_inplace_lshift */
        PyNull,                  /* nb_inplace_rshift */
        PyNull,                  /* nb_inplace_and */
        PyNull,                  /* nb_inplace_xor */
        PyNull,                  /* nb_inplace_or */
        mpreal_floor_div,    /* nb_floor_divide */
        mpreal_div,          /* nb_true_divide */
        PyNull,                  /* nb_inplace_floor_divide */
        PyNull,                  /* nb_inplace_true_divide */
};

PyTypeObject PyMpReal_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        "mpreal",
        sizeof(PyMpRealObject),
        0,
        (destructor) mpreal_dealloc,                  /* tp_dealloc */
        0,                                          /* tp_vectorcall_offset */
        PyNull,                                          /* tp_getattr */
        PyNull,                                          /* tp_setattr */
        PyNull,                                          /* tp_as_async */
        PyNull,                                          /* tp_repr */
        &MpRealNumberMethods,                         /* tp_as_number */
        PyNull,                                          /* tp_as_sequence */
        PyNull,                                          /* tp_as_mapping */
        (hashfunc) mpreal_hash,                      /* tp_hash */
        PyNull,                                          /* tp_call */
        PyNull,                                          /* tp_str */
        PyObject_GenericGetAttr,                    /* tp_getattro */
        PyNull,                                          /* tp_setattro */
        PyNull,                                          /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT,
        PyNull,                                          /* tp_doc */
        PyNull,                                          /* tp_traverse */
        PyNull,                                          /* tp_clear */
        mpreal_richcompare,                          /* tp_richcompare */
        0,                                          /* tp_weaklistoffset */
        PyNull,                                          /* tp_iter */
        PyNull,                                          /* tp_iternext */
        mpreal_methods,                             /* tp_methods */
        PyNull,                                          /* tp_members */
        PyNull,                              /* tp_getset */
        PyNull,                                          /* tp_base */
        PyNull,                                          /* tp_dict */
        PyNull,                                          /* tp_descr_get */
        PyNull,                                          /* tp_descr_set */
        0,                                          /* tp_dictoffset */
        PyNull,                                          /* tp_init */
        PyNull,                                          /* tp_alloc */
        mpreal_new,                                  /* tp_new */
};

#endif //QCALC_PYMPREAL_HPP
