#ifndef QCALC_NATIVEINTERFACE_HPP
#define QCALC_NATIVEINTERFACE_HPP

#include "view.hpp"

struct _object;
typedef _object PyObject;

/**
 * The set of functions exposed to python in a native module.
 */
class NativeInterface {
public:
    /**
     * Register a new native python module and link up the static member functions of this class to the module.
     * Should be called before initializing python.
     *
     * @param view The target view to invoke functions on.
     */
    static void initialize(View &view);

    static PyObject *showDialog(PyObject *self, PyObject *args);

private:
    static View *view;
};

#endif //QCALC_NATIVEINTERFACE_HPP
