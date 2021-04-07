# This module disconnects the input line edit from the main window and binds the signals to itself,
# and takes over the task of invoking the expression parser to enable addons to modify a shared symbol table.
import qcalc.gui as gui
import qcalc.exprtk as exprtk
import qcalc.numberformat as numberformat

from PySide2 import QtCore, QtGui

sym_table = exprtk.SymbolTable()
input_text = ""


class _WrapperWidget(QtCore.QObject):
    def on_input_text_changed(self, text):
        global input_text
        input_text = text

    def on_input_return_pressed(self):
        global input_text
        global sym_table
        result = exprtk.evaluate_with_side_effects(input_text, sym_table)
        result_str = numberformat.remove_decimal_spaces(numberformat.to_decimal(result[0]))
        sym_table = result[1]
        gui.history.addContent(input_text, result_str)
        input_text = result_str
        gui.input_line_edit.setText(input_text)


_wrapper = _WrapperWidget()


def _disconnect_main_window():
    gui.input_line_edit.disconnect(gui.wnd)


def _connect_main_window():
    global input_text
    gui.wnd.onInputTextChanged(input_text)
    QtCore.QObject.connect(gui.input_line_edit,
                           QtCore.SIGNAL("returnPressed()"),
                           gui.wnd,
                           QtCore.SLOT("onInputReturnPressed()"))
    QtCore.QObject.connect(gui.input_line_edit,
                           QtCore.SIGNAL("textChanged(QString)"),
                           gui.wnd,
                           QtCore.SLOT("onInputTextChanged(QString)"))


_users = 0


def enable_wrapper():
    global _users
    global _wrapper
    if _users == 0:
        _disconnect_main_window()
        QtCore.QObject.connect(gui.input_line_edit,
                               QtCore.SIGNAL("returnPressed()"),
                               _wrapper,
                               QtCore.SLOT("on_input_return_pressed()"))
        QtCore.QObject.connect(gui.input_line_edit,
                               QtCore.SIGNAL("textChanged(QString)"),
                               _wrapper,
                               QtCore.SLOT("on_input_text_changed(QString)"))
    _users = _users + 1


def disable_wrapper():
    global _users
    global _wrapper
    _users = _users - 1
    if _users == 0:
        gui.input_line_edit.disconnect(_wrapper)
        _connect_main_window()
