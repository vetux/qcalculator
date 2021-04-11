#ifndef QCALC_PYMPREAL_HPP
#define QCALC_PYMPREAL_HPP

#include "Python.h"

#include "extern/mpreal.h"

typedef struct {
    PyObject_HEAD
    mpfr::mpreal *mpreal; //Store a pointer to our c++ object because cpython does not like c++ constructors / destructors.
} PyMpRealObject;

PyObject *PyMpReal_FromMpReal(const mpfr::mpreal &val);

mpfr::mpreal PyMpReal_AsMpReal(PyObject *op);

PyObject *mpreal_richcompare(PyObject *v, PyObject *w, int op);

Py_hash_t mpreal_hash(PyMpRealObject *v);

PyObject *mpreal_add(PyObject *v, PyObject *w);

PyObject *mpreal_sub(PyObject *v, PyObject *w);

PyObject *mpreal_mul(PyObject *v, PyObject *w);

PyObject *mpreal_div(PyObject *v, PyObject *w);

PyObject *mpreal_rem(PyObject *v, PyObject *w);

PyObject *mpreal_divmod(PyObject *v, PyObject *w);

PyObject *mpreal_floor_div(PyObject *v, PyObject *w);

PyObject *mpreal_pow(PyObject *v, PyObject *w, PyObject *z);

PyObject *mpreal_neg(PyMpRealObject *v);

PyObject *mpreal_abs(PyMpRealObject *v);

int mpreal_bool(PyMpRealObject *v);

PyObject *mpreal_float(PyObject *v);

void mpreal_dealloc(PyMpRealObject *op);

PyObject *mpreal_str(PyObject *self);

int mpreal_init(PyObject *self, PyObject *args, PyObject *kwds);

PyObject *mpreal_new(PyTypeObject *type, PyObject *args, PyObject *kwds);

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
        PyNull,                      /* tp_hash */
        PyNull,                                          /* tp_call */
        mpreal_str,                                          /* tp_str */
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
        mpreal_init,                                          /* tp_init */
        PyNull,                                          /* tp_alloc */
        mpreal_new,                                  /* tp_new */
};

#define PyMpReal_Check(op) PyObject_TypeCheck(op, &PyMpReal_Type)

PyObject *PyMpReal_FromMpReal(const mpfr::mpreal &val) {
    auto *ret = static_cast<PyMpRealObject *>(PyObject_Malloc(sizeof(PyMpRealObject)));
    if (!ret) {
        return PyErr_NoMemory();
    }
    PyObject_Init((PyObject *) ret, &PyMpReal_Type); //PyObject_Init does not invoke tp_new or tp_init? Documentation?
    ret->mpreal = new mpfr::mpreal(val);
    return (PyObject *) ret;
}

mpfr::mpreal PyMpReal_AsMpReal(PyObject *op) {
    if (op == PyNull) {
        PyErr_BadArgument();
        return -1;
    }

    if (PyMpReal_Check(op)) {
        return *((PyMpRealObject *) (op))->mpreal;
    } else {
        PyErr_BadArgument();
        return -1;
    }
}


PyObject *mpreal_richcompare(PyObject *v, PyObject *w, int op) {
    if (!PyMpReal_Check(v) || !PyMpReal_Check(w)) {
        PyErr_BadArgument(); //Only allow operations between mpreal objects for now.
        return PyNull;
    }

    const mpfr::mpreal &a = *((PyMpRealObject *) v)->mpreal;
    const mpfr::mpreal &b = *((PyMpRealObject *) w)->mpreal;

    switch (op) {
        case Py_EQ:
            return PyBool_FromLong(a == b);
        case Py_NE:
            return PyBool_FromLong(a != b);
        case Py_LE:
            return PyBool_FromLong(a <= b);
        case Py_GE:
            return PyBool_FromLong(a >= b);
        case Py_LT:
            return PyBool_FromLong(a < b);
        case Py_GT:
            return PyBool_FromLong(a > b);
        default:
            PyErr_BadArgument(); // Should not happen unless new comparison operators are added.
            return PyNull;
    }
}

Py_hash_t mpreal_hash(PyMpRealObject *v) {
    //TODO: Implement hash function
    PyErr_SetString(PyExc_NotImplementedError, "Not Implemented");
    return -1;
}

PyObject *mpreal_add(PyObject *v, PyObject *w) {
    if (!PyMpReal_Check(v) || !PyMpReal_Check(w)) {
        PyErr_BadArgument(); //Only allow operations between mpreal objects for now.
        return PyNull;
    }
    const mpfr::mpreal &a = *((PyMpRealObject *) v)->mpreal;
    const mpfr::mpreal &b = *((PyMpRealObject *) w)->mpreal;
    return PyMpReal_FromMpReal(a + b);
}

PyObject *mpreal_sub(PyObject *v, PyObject *w) {
    if (!PyMpReal_Check(v) || !PyMpReal_Check(w)) {
        PyErr_BadArgument(); //Only allow operations between mpreal objects for now.
        return PyNull;
    }
    const mpfr::mpreal &a = *((PyMpRealObject *) v)->mpreal;
    const mpfr::mpreal &b = *((PyMpRealObject *) w)->mpreal;
    return PyMpReal_FromMpReal(a - b);
}

PyObject *mpreal_mul(PyObject *v, PyObject *w) {
    if (!PyMpReal_Check(v) || !PyMpReal_Check(w)) {
        PyErr_BadArgument(); //Only allow operations between mpreal objects for now.
        return PyNull;
    }
    const mpfr::mpreal &a = *((PyMpRealObject *) v)->mpreal;
    const mpfr::mpreal &b = *((PyMpRealObject *) w)->mpreal;
    return PyMpReal_FromMpReal(a * b);
}

PyObject *mpreal_div(PyObject *v, PyObject *w) {
    if (!PyMpReal_Check(v) || !PyMpReal_Check(w)) {
        PyErr_BadArgument(); //Only allow operations between mpreal objects for now.
        return PyNull;
    }
    const mpfr::mpreal &a = *((PyMpRealObject *) v)->mpreal;
    const mpfr::mpreal &b = *((PyMpRealObject *) w)->mpreal;
    return PyMpReal_FromMpReal(a / b);
}

PyObject *mpreal_rem(PyObject *v, PyObject *w) {
    //TODO: Implement remainder
    PyErr_SetString(PyExc_NotImplementedError, "Not Implemented");
    return PyNull;
}

PyObject *mpreal_divmod(PyObject *v, PyObject *w) {
    //TODO: Implement divmod
    PyErr_SetString(PyExc_NotImplementedError, "Not Implemented");
    return PyNull;
}

PyObject *mpreal_floor_div(PyObject *v, PyObject *w) {
    //TODO: Implement floor divide
    PyErr_SetString(PyExc_NotImplementedError, "Not Implemented");
    return PyNull;
}

PyObject *mpreal_pow(PyObject *v, PyObject *w, PyObject *z) {
    //TODO: Implement modulus support for pow
    if (!PyMpReal_Check(v) || !PyMpReal_Check(w) || z != PyNull) {
        PyErr_BadArgument(); //Only allow operations between mpreal objects for now.
        return PyNull;
    }
    const mpfr::mpreal &a = *((PyMpRealObject *) v)->mpreal;
    const mpfr::mpreal &b = *((PyMpRealObject *) w)->mpreal;
    return PyMpReal_FromMpReal(mpfr::pow(a, b));
}

PyObject *mpreal_neg(PyMpRealObject *v) {
    if (!PyMpReal_Check(v)) {
        PyErr_BadArgument(); //Only allow operations between mpreal objects for now.
        return PyNull;
    }
    return PyMpReal_FromMpReal((*v->mpreal) * -1);
}

PyObject *mpreal_abs(PyMpRealObject *v) {
    if (!PyMpReal_Check(v)) {
        PyErr_BadArgument(); //Only allow operations between mpreal objects for now.
        return PyNull;
    }
    return PyMpReal_FromMpReal(mpfr::abs(*v->mpreal));
}

int mpreal_bool(PyMpRealObject *v) {
    return *v->mpreal != 0;
}

PyObject *mpreal_float(PyObject *v) {
    if (!PyMpReal_Check(v)) {
        PyErr_BadInternalCall(); // Should never happen.
        return PyNull;
    }
    return PyFloat_FromDouble(((PyMpRealObject *) v)->mpreal->toDouble());
}

void mpreal_dealloc(PyMpRealObject *op) {
    delete op->mpreal; // Invoke c++ destructor on our pointer.
    Py_TYPE(op)->tp_free((PyObject *) op);
}

PyObject *mpreal_str(PyObject *self) {
    return PyUnicode_FromString(((PyMpRealObject *) self)->mpreal->toString().c_str());
}

int mpreal_init(PyObject *self, PyObject *args, PyObject *kwds) {
    PyObject *arg = PyNull;
    if (!PyArg_ParseTuple(args, "|O:", &arg))
        return -1;

    if (arg != PyNull) {
        if (PyMpReal_Check(arg)) {
            ((PyMpRealObject *) self)->mpreal = new mpfr::mpreal(PyMpReal_AsMpReal(arg));
        } else if (PyFloat_Check(arg)) {
            ((PyMpRealObject *) self)->mpreal = new mpfr::mpreal(PyFloat_AsDouble(arg));
        } else if (PyLong_Check(arg)) {
            ((PyMpRealObject *) self)->mpreal = new mpfr::mpreal(PyLong_AsLong(arg));
        } else {
            PyErr_BadArgument();
            return -1;
        }
    } else {
        ((PyMpRealObject *) self)->mpreal = 0;
    }

    return 0;
}

PyObject *mpreal_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    auto *self = (PyMpRealObject *) type->tp_alloc(type, 0);
    if (self != PyNull) {
        self->mpreal = nullptr; //Store nullptr in new and allocate the c++ object in the init callback.
    }
    return (PyObject *) self;
}

#endif //QCALC_PYMPREAL_HPP
