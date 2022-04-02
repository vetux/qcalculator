# QCalc - Extensible programming calculator
# Copyright (C) 2021  Julian Zampiccoli
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

import qcalc.gui as gui
from qcalc.exprtk import mpreal as mpreal

from PySide2 import QtCore, QtWidgets

uint_max = 18446744073709551615


# https://stackoverflow.com/a/57814974
def bool2int(bools):
    return int(''.join(str(int(i)) for i in reversed(bools)), 2)


# https://stackoverflow.com/a/33608387
def int_to_bool_list(num):
    return [bool(num & (1 << n)) for n in range(64)]


# https://stackoverflow.com/q/354038
def is_number(s):
    try:
        mpreal(s)
        return True
    except ValueError:
        return False


class BitButton(QtWidgets.QPushButton):
    def __init__(self, parent):
        QtWidgets.QPushButton.__init__(self, parent)
        self.index = 0
        self.toggle_value = False
        QtCore.QObject.connect(self, QtCore.SIGNAL("pressed()"), self, QtCore.SLOT("slot_button_pressed()"))
        self.setMinimumSize(QtCore.QSize(15, 30))
        self.setMaximumSize(QtCore.QSize(30, 60))
        self.setStyleSheet(
            "QPushButton{border: 0;background: transparent;}QPushButton:hover {border: 1px solid palette(highlight);background: transparent;}")

    def set_toggle(self, toggle):
        self.toggle_value = toggle
        if toggle:
            self.setText("1")
        else:
            self.setText("0")

    def toggle(self):
        if self.toggle_value:
            self.set_toggle(False)
        else:
            self.set_toggle(True)

    def slot_button_pressed(self):
        self.signal_bit_pressed.emit(self.index)

    signal_bit_pressed = QtCore.Signal("int")


class BitViewWidget(QtWidgets.QWidget):
    def __init__(self, parent):
        QtWidgets.QWidget.__init__(self, parent)
        self.buttons = []
        self.setLayout(QtWidgets.QVBoxLayout())
        for i in range(64):
            self.buttons.append(None)
        hbox0 = QtWidgets.QHBoxLayout()
        hbox1 = QtWidgets.QHBoxLayout()
        bitgroup_16 = QtWidgets.QHBoxLayout()
        bitgroup_15 = QtWidgets.QHBoxLayout()
        bitgroup_14 = QtWidgets.QHBoxLayout()
        bitgroup_13 = QtWidgets.QHBoxLayout()
        bitgroup_12 = QtWidgets.QHBoxLayout()
        bitgroup_11 = QtWidgets.QHBoxLayout()
        bitgroup_10 = QtWidgets.QHBoxLayout()
        bitgroup_9 = QtWidgets.QHBoxLayout()
        bitgroup_8 = QtWidgets.QHBoxLayout()
        bitgroup_7 = QtWidgets.QHBoxLayout()
        bitgroup_6 = QtWidgets.QHBoxLayout()
        bitgroup_5 = QtWidgets.QHBoxLayout()
        bitgroup_4 = QtWidgets.QHBoxLayout()
        bitgroup_3 = QtWidgets.QHBoxLayout()
        bitgroup_2 = QtWidgets.QHBoxLayout()
        bitgroup_1 = QtWidgets.QHBoxLayout()
        margin = 9
        bitgroup_16.setMargin(margin)
        bitgroup_15.setMargin(margin)
        bitgroup_14.setMargin(margin)
        bitgroup_13.setMargin(margin)
        bitgroup_12.setMargin(margin)
        bitgroup_11.setMargin(margin)
        bitgroup_10.setMargin(margin)
        bitgroup_9.setMargin(margin)
        bitgroup_8.setMargin(margin)
        bitgroup_7.setMargin(margin)
        bitgroup_6.setMargin(margin)
        bitgroup_5.setMargin(margin)
        bitgroup_4.setMargin(margin)
        bitgroup_3.setMargin(margin)
        bitgroup_2.setMargin(margin)
        bitgroup_1.setMargin(margin)
        self.instantiate_bit_widgets(63, 59, bitgroup_16)
        self.instantiate_bit_widgets(59, 55, bitgroup_15)
        self.instantiate_bit_widgets(55, 51, bitgroup_14)
        self.instantiate_bit_widgets(51, 47, bitgroup_13)
        self.instantiate_bit_widgets(47, 43, bitgroup_12)
        self.instantiate_bit_widgets(43, 39, bitgroup_11)
        self.instantiate_bit_widgets(39, 35, bitgroup_10)
        self.instantiate_bit_widgets(35, 31, bitgroup_9)
        self.instantiate_bit_widgets(31, 27, bitgroup_8)
        self.instantiate_bit_widgets(27, 23, bitgroup_7)
        self.instantiate_bit_widgets(23, 19, bitgroup_6)
        self.instantiate_bit_widgets(19, 15, bitgroup_5)
        self.instantiate_bit_widgets(15, 11, bitgroup_4)
        self.instantiate_bit_widgets(11, 7, bitgroup_3)
        self.instantiate_bit_widgets(7, 3, bitgroup_2)
        self.instantiate_bit_widgets(3, -1, bitgroup_1)
        hbox0.addLayout(bitgroup_16)
        hbox0.addLayout(bitgroup_15)
        hbox0.addLayout(bitgroup_14)
        hbox0.addLayout(bitgroup_13)
        hbox0.addLayout(bitgroup_12)
        hbox0.addLayout(bitgroup_11)
        hbox0.addLayout(bitgroup_10)
        hbox0.addLayout(bitgroup_9)
        hbox1.addLayout(bitgroup_8)
        hbox1.addLayout(bitgroup_7)
        hbox1.addLayout(bitgroup_6)
        hbox1.addLayout(bitgroup_5)
        hbox1.addLayout(bitgroup_4)
        hbox1.addLayout(bitgroup_3)
        hbox1.addLayout(bitgroup_2)
        hbox1.addLayout(bitgroup_1)
        self.layout().addLayout(hbox0)
        self.layout().addLayout(hbox1)

    def instantiate_bit_widgets(self, index, end, layout):
        while index > end:
            button = BitButton(self)
            button.index = index
            button.set_toggle(False)
            QtCore.QObject.connect(button, QtCore.SIGNAL("signal_bit_pressed(int)"), self,
                                   QtCore.SLOT("slot_bit_button_pressed(int)"))
            layout.addWidget(button)
            self.buttons[index] = button
            index = index - 1

    def get_bits_value(self):
        bools = []
        for i in range(64):
            bools.append(None)
        for button in self.buttons:
            bools[button.index] = button.toggle_value
        return bool2int(bools)

    def set_bits_value(self, value):
        bools = int_to_bool_list(value)
        i = 0
        while i < 64:
            if i >= len(bools):
                self.buttons[i].set_toggle(False)
            else:
                self.buttons[i].set_toggle(bools[i])
            i = i + 1

    def parse_str(self, value_string):
        if len(value_string) == 0:
            self.set_bits_value(0)
            self.setEnabled(True)
            return
        if not is_number(value_string):
            self.set_bits_value(0)
            self.setEnabled(False)
            return
        value = mpreal(value_string)
        if value < mpreal(0):
            self.set_bits_value(0)
            self.setEnabled(False)
            return
        if not value.is_integer():
            self.set_bits_value(0)
            self.setEnabled(False)
            return
        i = int(value)
        if i > uint_max:
            self.set_bits_value(0)
            self.setEnabled(False)
            return
        self.setEnabled(True)
        self.set_bits_value(i)

    def slot_bit_button_pressed(self, bit_index):
        self.buttons[bit_index].toggle()
        v = self.get_bits_value()
        self.signal_set_input_text.emit(str(v))

    def slot_expression_evaluated(self, expression, value_string):
        self.parse_str(value_string)

    def slot_input_text_changed(self, text):
        self.parse_str(text)

    signal_set_input_text = QtCore.Signal("QString")


widget = BitViewWidget


def load():
    global widget
    widget = BitViewWidget(gui.wnd)
    QtWidgets.QWidget.connect(gui.wnd, QtCore.SIGNAL("signalExpressionEvaluated(const QString &, const QString &)"),
                              widget, QtCore.SLOT("slot_expression_evaluated(QString, QString)"))
    QtWidgets.QWidget.connect(gui.input_line_edit, QtCore.SIGNAL("textChanged(const QString &)"), widget,
                              QtCore.SLOT("slot_input_text_changed(QString)"))
    QtWidgets.QWidget.connect(widget, QtCore.SIGNAL("signal_set_input_text(QString)"), gui.input_line_edit,
                              QtCore.SLOT("setText(QString)"))
    QtWidgets.QWidget.connect(widget, QtCore.SIGNAL("signal_set_input_text(QString)"), gui.wnd,
                              QtCore.SLOT("onInputReturnPressed()"))

    gui.root.layout().insertWidget(2, widget)

    widget.slot_input_text_changed(gui.input_line_edit.text())


def unload():
    global widget
    gui.root.layout().removeWidget(widget)
    gui.wnd.disconnect(widget)
    gui.input_line_edit.disconnect(widget)

    widget.disconnect(gui.input_line_edit)
    widget.deleteLater()
