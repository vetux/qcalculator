#ifndef QCALC_NATIVEINTERFACE_HPP
#define QCALC_NATIVEINTERFACE_HPP

#include "gui/mainview.hpp"

struct _object;
typedef _object PyObject;

/**
 * The set of functions exposed to python in a native module.
 *
 * The native interface interacts with the MainView.
 *
 * In this way anything interacting with the native interface basically acts as a presenter extension.
 */
class NativeInterface {
public:
    /**
     * Register a new native python module and link up the static member functions of this class to the module.
     * Should be called before initializing python.
     *
     * @param view The target view to invoke functions on.
     */
    static void initialize(MainView &view);

    static PyObject *showDialog(PyObject *self, PyObject *args);

private:
    static MainView *view;
};

#endif //QCALC_NATIVEINTERFACE_HPP
