#ifndef QCALC_NATIVEINTERFACE_HPP
#define QCALC_NATIVEINTERFACE_HPP

#include "gui/mainview.hpp"
#include "gui/mainpresenter.hpp"

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
     * @param view The target view
     * @param presenter The target presenter
     */
    static void initialize(MainView &view, MainPresenter& presenter);

    static PyObject *registerScriptFunction(PyObject *self, PyObject *args);

    static PyObject *unregisterScriptFunction(PyObject *self, PyObject *args);

private:
    static MainView *view;
    static MainPresenter *presenter;
};

#endif //QCALC_NATIVEINTERFACE_HPP
