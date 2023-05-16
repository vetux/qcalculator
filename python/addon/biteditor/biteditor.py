"""
{
  "displayName": "Bit Editor",
  "description": "Edit binary representations of numbers with toggleable bits."
}
"""

# QCalc - Extensible programming calculator
# Copyright (C) 2023  Julian Zampiccoli
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

#TODO: Fix bitview addon bitgroup widgets not deleting when unloading the addon.

import qcalc as gui
import decimal

from PySide2 import QtCore, QtWidgets

max_8bit = 255
max_16bit = 65535
max_32bit = 4294967295
max_64bit = 18446744073709551615


# https://stackoverflow.com/a/57814974
def bool2int(bools):
    return int(''.join(str(int(i)) for i in reversed(bools)), 2)


# https://stackoverflow.com/a/33608387
def int_to_bool_list(num):
    return [bool(num & (1 << n)) for n in range(64)]


def is_number(s):
    try:
        return not decimal.Decimal(s).is_nan()
    except decimal.InvalidOperation:
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
        return self.toggle_value

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
        selectionBox = QtWidgets.QComboBox()
        selectionBox.addItem("8")
        selectionBox.addItem("16")
        selectionBox.addItem("32")
        selectionBox.addItem("64")
        QtCore.QObject.connect(selectionBox, QtCore.SIGNAL("currentIndexChanged(int)"), self,
                               QtCore.SLOT("slot_current_index_changed(int)"))
        self.layout().addWidget(selectionBox)
        self.hbox0.setLayout(QtWidgets.QHBoxLayout())
        self.hbox1.setLayout(QtWidgets.QHBoxLayout())
        self.hbox0.layout().setMargin(0)
        self.hbox1.layout().setMargin(0)
        self.bitgroup_16.setLayout(QtWidgets.QHBoxLayout())
        self.bitgroup_15.setLayout(QtWidgets.QHBoxLayout())
        self.bitgroup_14.setLayout(QtWidgets.QHBoxLayout())
        self.bitgroup_13.setLayout(QtWidgets.QHBoxLayout())
        self.bitgroup_12.setLayout(QtWidgets.QHBoxLayout())
        self.bitgroup_11.setLayout(QtWidgets.QHBoxLayout())
        self.bitgroup_10.setLayout(QtWidgets.QHBoxLayout())
        self.bitgroup_9.setLayout(QtWidgets.QHBoxLayout())
        self.bitgroup_8.setLayout(QtWidgets.QHBoxLayout())
        self.bitgroup_7.setLayout(QtWidgets.QHBoxLayout())
        self.bitgroup_6.setLayout(QtWidgets.QHBoxLayout())
        self.bitgroup_5.setLayout(QtWidgets.QHBoxLayout())
        self.bitgroup_4.setLayout(QtWidgets.QHBoxLayout())
        self.bitgroup_3.setLayout(QtWidgets.QHBoxLayout())
        self.bitgroup_2.setLayout(QtWidgets.QHBoxLayout())
        self.bitgroup_1.setLayout(QtWidgets.QHBoxLayout())
        margin = 9
        self.bitgroup_16.layout().setMargin(margin)
        self.bitgroup_15.layout().setMargin(margin)
        self.bitgroup_14.layout().setMargin(margin)
        self.bitgroup_13.layout().setMargin(margin)
        self.bitgroup_12.layout().setMargin(margin)
        self.bitgroup_11.layout().setMargin(margin)
        self.bitgroup_10.layout().setMargin(margin)
        self.bitgroup_9.layout().setMargin(margin)
        self.bitgroup_8.layout().setMargin(margin)
        self.bitgroup_7.layout().setMargin(margin)
        self.bitgroup_6.layout().setMargin(margin)
        self.bitgroup_5.layout().setMargin(margin)
        self.bitgroup_4.layout().setMargin(margin)
        self.bitgroup_3.layout().setMargin(margin)
        self.bitgroup_2.layout().setMargin(margin)
        self.bitgroup_1.layout().setMargin(margin)
        self.instantiate_bit_widgets(63, 59, self.bitgroup_16)
        self.instantiate_bit_widgets(59, 55, self.bitgroup_15)
        self.instantiate_bit_widgets(55, 51, self.bitgroup_14)
        self.instantiate_bit_widgets(51, 47, self.bitgroup_13)
        self.instantiate_bit_widgets(47, 43, self.bitgroup_12)
        self.instantiate_bit_widgets(43, 39, self.bitgroup_11)
        self.instantiate_bit_widgets(39, 35, self.bitgroup_10)
        self.instantiate_bit_widgets(35, 31, self.bitgroup_9)
        self.instantiate_bit_widgets(31, 27, self.bitgroup_8)
        self.instantiate_bit_widgets(27, 23, self.bitgroup_7)
        self.instantiate_bit_widgets(23, 19, self.bitgroup_6)
        self.instantiate_bit_widgets(19, 15, self.bitgroup_5)
        self.instantiate_bit_widgets(15, 11, self.bitgroup_4)
        self.instantiate_bit_widgets(11, 7, self.bitgroup_3)
        self.instantiate_bit_widgets(7, 3, self.bitgroup_2)
        self.instantiate_bit_widgets(3, -1, self.bitgroup_1)
        self.hbox0.layout().addWidget(self.bitgroup_16)
        self.hbox0.layout().addWidget(self.bitgroup_15)
        self.hbox0.layout().addWidget(self.bitgroup_14)
        self.hbox0.layout().addWidget(self.bitgroup_13)
        self.hbox0.layout().addWidget(self.bitgroup_12)
        self.hbox0.layout().addWidget(self.bitgroup_11)
        self.hbox0.layout().addWidget(self.bitgroup_10)
        self.hbox0.layout().addWidget(self.bitgroup_9)
        self.hbox1.layout().addWidget(self.bitgroup_8)
        self.hbox1.layout().addWidget(self.bitgroup_7)
        self.hbox1.layout().addWidget(self.bitgroup_6)
        self.hbox1.layout().addWidget(self.bitgroup_5)
        self.hbox1.layout().addWidget(self.bitgroup_4)
        self.hbox1.layout().addWidget(self.bitgroup_3)
        self.hbox1.layout().addWidget(self.bitgroup_2)
        self.hbox1.layout().addWidget(self.bitgroup_1)
        self.layout().addWidget(self.hbox0)
        self.layout().addWidget(self.hbox1)

        for i in range(8):
            self.hbox0.layout().itemAt(i).widget().layout().setSpacing(0)
            self.hbox0.layout().itemAt(i).widget().layout().setMargin(5)
        for i in range(8):
            self.hbox1.layout().itemAt(i).widget().layout().setSpacing(0)
            self.hbox1.layout().itemAt(i).widget().layout().setMargin(5)

        self.set_bit_count(8)

    def set_bit_count(self, count):
        self.bitgroup_1.setVisible(False)
        self.bitgroup_2.setVisible(False)
        self.bitgroup_3.setVisible(False)
        self.bitgroup_4.setVisible(False)
        self.bitgroup_5.setVisible(False)
        self.bitgroup_6.setVisible(False)
        self.bitgroup_7.setVisible(False)
        self.bitgroup_8.setVisible(False)
        self.bitgroup_9.setVisible(False)
        self.bitgroup_10.setVisible(False)
        self.bitgroup_11.setVisible(False)
        self.bitgroup_12.setVisible(False)
        self.bitgroup_13.setVisible(False)
        self.bitgroup_14.setVisible(False)
        self.bitgroup_15.setVisible(False)
        self.bitgroup_16.setVisible(False)
        self.hbox0.setVisible(False)
        self.hbox1.setVisible(False)
        if (count <= 8):
            self.bitcount = 8
            self.hbox1.setVisible(True)
            self.bitgroup_1.setVisible(True)
            self.bitgroup_2.setVisible(True)
        elif(count <= 16):
            self.bitcount = 16
            self.hbox1.setVisible(True)
            self.bitgroup_1.setVisible(True)
            self.bitgroup_2.setVisible(True)
            self.bitgroup_3.setVisible(True)
            self.bitgroup_4.setVisible(True)
        elif(count <= 32):
            self.bitcount = 32
            self.hbox1.setVisible(True)
            self.bitgroup_1.setVisible(True)
            self.bitgroup_2.setVisible(True)
            self.bitgroup_3.setVisible(True)
            self.bitgroup_4.setVisible(True)
            self.bitgroup_5.setVisible(True)
            self.bitgroup_6.setVisible(True)
            self.bitgroup_7.setVisible(True)
            self.bitgroup_8.setVisible(True)
        elif(count <= 64):
            self.bitcount = 64
            self.hbox0.setVisible(True)
            self.hbox1.setVisible(True)
            self.bitgroup_1.setVisible(True)
            self.bitgroup_2.setVisible(True)
            self.bitgroup_3.setVisible(True)
            self.bitgroup_4.setVisible(True)
            self.bitgroup_5.setVisible(True)
            self.bitgroup_6.setVisible(True)
            self.bitgroup_7.setVisible(True)
            self.bitgroup_8.setVisible(True)
            self.bitgroup_9.setVisible(True)
            self.bitgroup_10.setVisible(True)
            self.bitgroup_11.setVisible(True)
            self.bitgroup_12.setVisible(True)
            self.bitgroup_13.setVisible(True)
            self.bitgroup_14.setVisible(True)
            self.bitgroup_15.setVisible(True)
            self.bitgroup_16.setVisible(True)

    def instantiate_bit_widgets(self, index, end, layout):
        while index > end:
            button = BitButton(self)
            button.index = index
            button.set_toggle(False)
            QtCore.QObject.connect(button, QtCore.SIGNAL("signal_bit_pressed(int)"), self,
                                   QtCore.SLOT("slot_bit_button_pressed(int)"))
            layout.layout().addWidget(button)
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
            self.hbox0.setEnabled(True)
            self.hbox1.setEnabled(True)
            return
        if not is_number(value_string):
            self.set_bits_value(0)
            self.hbox0.setEnabled(False)
            self.hbox1.setEnabled(False)
            return
        value = decimal.Decimal(value_string)
        if value < decimal.Decimal(0):
            self.set_bits_value(0)
            self.hbox0.setEnabled(False)
            self.hbox1.setEnabled(False)
            return
        if value != value.to_integral_value():
            self.set_bits_value(0)
            self.hbox0.setEnabled(False)
            self.hbox1.setEnabled(False)
            return
        i = int(value)
        if ((self.bitcount == 8 and i > max_8bit)
                or (self.bitcount == 16 and i > max_16bit)
                or (self.bitcount == 32 and i > max_32bit)
                or (self.bitcount == 64 and i > max_64bit)):
            self.set_bits_value(0)
            self.hbox0.setEnabled(False)
            self.hbox1.setEnabled(False)
            return
        self.hbox0.setEnabled(True)
        self.hbox1.setEnabled(True)
        self.set_bits_value(i)

    def slot_bit_button_pressed(self, bit_index):
        modifiers = QtWidgets.QApplication.keyboardModifiers()
        if modifiers == QtCore.Qt.ControlModifier:
            for i in range(0, 64):
                self.buttons[i].set_toggle(False)
            for i in range(0, bit_index + 1):
                self.buttons[i].set_toggle(True)
        else:
            self.buttons[bit_index].toggle()
        v = self.get_bits_value()
        self.signal_set_input_text.emit(str(v))

    def slot_current_index_changed(self, index):
        if index == 0:
            self.set_bit_count(8)
        elif index == 1:
            self.set_bit_count(16)
        elif index == 2:
            self.set_bit_count(32)
        elif index == 3:
            self.set_bit_count(64)
        self.parse_str(gui.input_line_edit.text())

    def slot_expression_evaluated(self, expression, value_string):
        self.parse_str(value_string)

    def slot_input_text_changed(self, text):
        self.parse_str(text)

    signal_set_input_text = QtCore.Signal("QString")
    bitgroup_16 = QtWidgets.QWidget()
    bitgroup_15 = QtWidgets.QWidget()
    bitgroup_14 = QtWidgets.QWidget()
    bitgroup_13 = QtWidgets.QWidget()
    bitgroup_12 = QtWidgets.QWidget()
    bitgroup_11 = QtWidgets.QWidget()
    bitgroup_10 = QtWidgets.QWidget()
    bitgroup_9 = QtWidgets.QWidget()
    bitgroup_8 = QtWidgets.QWidget()
    bitgroup_7 = QtWidgets.QWidget()
    bitgroup_6 = QtWidgets.QWidget()
    bitgroup_5 = QtWidgets.QWidget()
    bitgroup_4 = QtWidgets.QWidget()
    bitgroup_3 = QtWidgets.QWidget()
    bitgroup_2 = QtWidgets.QWidget()
    bitgroup_1 = QtWidgets.QWidget()
    hbox0 = QtWidgets.QWidget()
    hbox1 = QtWidgets.QWidget()
    bitcount = 8

widget = BitViewWidget

containerWidget = QtWidgets.QWidget

def load():
    global containerWidget
    global widget

    containerWidget = QtWidgets.QWidget(gui.wnd)
    widget = BitViewWidget(containerWidget)

    containerWidget.setLayout(QtWidgets.QHBoxLayout())
    containerWidget.layout().addWidget(widget, 1)
    containerWidget.layout().setMargin(0)

    QtWidgets.QWidget.connect(gui.wnd,
                              QtCore.SIGNAL("signalExpressionEvaluated(const QString &, const QString &)"),
                              widget,
                              QtCore.SLOT("slot_expression_evaluated(QString, QString)"))
    QtWidgets.QWidget.connect(gui.input_line_edit,
                              QtCore.SIGNAL("textChanged(const QString &)"),
                              widget,
                              QtCore.SLOT("slot_input_text_changed(QString)"))
    QtWidgets.QWidget.connect(widget,
                              QtCore.SIGNAL("signal_set_input_text(QString)"),
                              gui.input_line_edit,
                              QtCore.SLOT("setText(QString)"))

    gui.insert_widget_footer(containerWidget, 1, 0)

    widget.slot_input_text_changed(gui.input_line_edit.text())

def unload():
    global containerWidget
    global widget
    gui.remove_widget_footer(containerWidget)

    gui.wnd.disconnect(widget)
    gui.input_line_edit.disconnect(widget)

    widget.disconnect(gui.input_line_edit)
    widget.deleteLater()

    containerWidget.deleteLater()
