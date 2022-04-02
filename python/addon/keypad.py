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
from PySide2 import QtCore, QtWidgets

from qcalc.exprtk import mpreal as mpreal
import qcalc.mprealformatting

class KeyCode:
    START = 0

    KEY_0 = 0
    KEY_1 = 1
    KEY_2 = 2
    KEY_3 = 3
    KEY_4 = 4
    KEY_5 = 5
    KEY_6 = 6
    KEY_7 = 7
    KEY_8 = 8
    KEY_9 = 9

    KEY_DOT = 10
    KEY_DIVIDE = 11
    KEY_MULTIPLY = 12
    KEY_ADD = 13
    KEY_SUBTRACT = 14
    KEY_OPENBRACKET = 15
    KEY_CLOSEBRACKET = 16

    KEY_EQUAL = 17

    END = 18


def convert_key_to_string(key):
    if key == KeyCode.KEY_0:
        return "0"
    elif key == KeyCode.KEY_1:
        return "1"
    elif key == KeyCode.KEY_2:
        return "2"
    elif key == KeyCode.KEY_3:
        return "3"
    elif key == KeyCode.KEY_4:
        return "4"
    elif key == KeyCode.KEY_5:
        return "5"
    elif key == KeyCode.KEY_6:
        return "6"
    elif key == KeyCode.KEY_7:
        return "7"
    elif key == KeyCode.KEY_8:
        return "8"
    elif key == KeyCode.KEY_9:
        return "9"
    elif key == KeyCode.KEY_DOT:
        return "."
    elif key == KeyCode.KEY_DIVIDE:
        return "/"
    elif key == KeyCode.KEY_MULTIPLY:
        return "*"
    elif key == KeyCode.KEY_ADD:
        return "+"
    elif key == KeyCode.KEY_SUBTRACT:
        return "-"
    elif key == KeyCode.KEY_OPENBRACKET:
        return "("
    elif key == KeyCode.KEY_CLOSEBRACKET:
        return ")"
    elif key == KeyCode.KEY_EQUAL:
        return "="
    else:
        raise RuntimeError("Unknown key code " + str(key))


def get_key_position(key):
    if key == KeyCode.KEY_0:
        return 4, 1
    elif key == KeyCode.KEY_1:
        return 3, 0
    elif key == KeyCode.KEY_2:
        return 3, 1
    elif key == KeyCode.KEY_3:
        return 3, 2
    elif key == KeyCode.KEY_4:
        return 2, 0
    elif key == KeyCode.KEY_5:
        return 2, 1
    elif key == KeyCode.KEY_6:
        return 2, 2
    elif key == KeyCode.KEY_7:
        return 1, 0
    elif key == KeyCode.KEY_8:
        return 1, 1
    elif key == KeyCode.KEY_9:
        return 1, 2
    elif key == KeyCode.KEY_DOT:
        return 0, 2
    elif key == KeyCode.KEY_DIVIDE:
        return 0, 3
    elif key == KeyCode.KEY_MULTIPLY:
        return 1, 3
    elif key == KeyCode.KEY_ADD:
        return 3, 3
    elif key == KeyCode.KEY_SUBTRACT:
        return 2, 3
    elif key == KeyCode.KEY_OPENBRACKET:
        return 0, 0
    elif key == KeyCode.KEY_CLOSEBRACKET:
        return 0, 1
    elif key == KeyCode.KEY_EQUAL:
        return 4, 3
    else:
        raise RuntimeError("Unknown key code " + str(key))


class KeyPadButton(QtWidgets.QPushButton):
    def __init__(self, parent):
        QtWidgets.QPushButton.__init__(self, parent)
        self.key = KeyCode.KEY_0
        QtCore.QObject.connect(self, QtCore.SIGNAL("pressed()"), self, QtCore.SLOT("slot_key_pressed()"))

    def slot_key_pressed(self):
        self.signal_key_pressed.emit(self.key)

    signal_key_pressed = QtCore.Signal("int")


class KeyPadWidget(QtWidgets.QWidget):
    def __init__(self, parent, font=gui.input_line_edit.font()):
        QtWidgets.QWidget.__init__(self, parent)
        self.setLayout(QtWidgets.QHBoxLayout())
        self.layout().setMargin(0)
        container = QtWidgets.QWidget(self)
        self.layout().addWidget(container)
        container.setLayout(QtWidgets.QGridLayout())
        container.layout().setMargin(0)
        i = KeyCode.START
        while i < KeyCode.END:
            btn = KeyPadButton(self)
            btn.key = i
            btn.setText(convert_key_to_string(i))
            btn.setFont(font)
            pos = get_key_position(i)
            container.layout().addWidget(btn, pos[0], pos[1])
            QtCore.QObject.connect(btn,
                                   QtCore.SIGNAL("signal_key_pressed(int)"),
                                   self,
                                   QtCore.SIGNAL("signal_key_pressed(int)"))
            i = i + 1
        btn = QtWidgets.QPushButton()
        btn.setText("")
        btn.setFont(font)
        btn.setEnabled(False)
        container.layout().addWidget(btn, 4, 0)

        btn = QtWidgets.QPushButton()
        btn.setText("")
        btn.setFont(font)
        btn.setEnabled(False)
        container.layout().addWidget(btn, 4, 2)
    signal_key_pressed = QtCore.Signal("int")


class KeyPadHandler(QtCore.QObject):
    def __init__(self):
        QtCore.QObject.__init__(self)

    def slot_key_press(self, keycode):
        if keycode == KeyCode.KEY_EQUAL:
            gui.input_line_edit.returnPressed.emit()
        else:
            v = convert_key_to_string(keycode)
            t = gui.input_line_edit.text()
            t += v
            gui.input_line_edit.setText(t)

class NumeralSystemWidget(QtWidgets.QWidget):
    def __init__(self, parent):
        QtWidgets.QWidget.__init__(self, parent)
        self.setLayout(QtWidgets.QVBoxLayout())

        self.layout().setSpacing(12)
        self.layout().setMargin(12)

        declayout = QtWidgets.QHBoxLayout()
        label = QtWidgets.QLabel(self)
        label.setText("Decimal")
        label.setSizePolicy(QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Preferred))
        label.setMinimumSize(150, 0)
        label.setAlignment(QtCore.Qt.AlignCenter)
        self.dectext = QtWidgets.QLineEdit(self)
        declayout.addWidget(label)
        declayout.addWidget(self.dectext)

        hexlayout = QtWidgets.QHBoxLayout()
        label = QtWidgets.QLabel(self)
        label.setText("Hexadecimal")
        label.setSizePolicy(QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Preferred))
        label.setMinimumSize(150, 0)
        label.setAlignment(QtCore.Qt.AlignCenter)
        self.hextext = QtWidgets.QLineEdit(self)
        hexlayout.addWidget(label)
        hexlayout.addWidget(self.hextext)

        octlayout = QtWidgets.QHBoxLayout()
        label = QtWidgets.QLabel(self)
        label.setText("Octal")
        label.setSizePolicy(QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Preferred))
        label.setMinimumSize(150, 0)
        label.setAlignment(QtCore.Qt.AlignCenter)
        self.octtext = QtWidgets.QLineEdit(self)
        octlayout.addWidget(label)
        octlayout.addWidget(self.octtext)

        binlayout = QtWidgets.QHBoxLayout()
        label = QtWidgets.QLabel(self)
        label.setText("Binary")
        label.setSizePolicy(QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Preferred))
        label.setMinimumSize(150, 0)
        label.setAlignment(QtCore.Qt.AlignCenter)
        self.bintext = QtWidgets.QLineEdit(self)
        binlayout.addWidget(label)
        binlayout.addWidget(self.bintext)

        self.layout().addLayout(declayout)
        self.layout().addLayout(hexlayout)
        self.layout().addLayout(octlayout)
        self.layout().addLayout(binlayout)

        QtCore.QObject.connect(self.dectext,
                               QtCore.SIGNAL("editingFinished()"),
                               self,
                               QtCore.SLOT("slot_decimal_editing_finished()"))
        QtCore.QObject.connect(self.hextext,
                               QtCore.SIGNAL("editingFinished()"),
                               self,
                               QtCore.SLOT("slot_hex_editing_finished()"))
        QtCore.QObject.connect(self.octtext,
                               QtCore.SIGNAL("editingFinished()"),
                               self,
                               QtCore.SLOT("slot_octal_editing_finished()"))
        QtCore.QObject.connect(self.bintext,
                               QtCore.SIGNAL("editingFinished()"),
                               self,
                               QtCore.SLOT("slot_binary_editing_finished()"))

    def slot_decimal_editing_finished(self):
        if self.dectext.isModified():
            try:
                self.slot_set_value(qcalc.mprealformatting.from_decimal(self.dectext.text()))
            except:
                self.dectext.setText("")
                self.hextext.setText("")
                self.octtext.setText("")
                self.bintext.setText("")

    def slot_hex_editing_finished(self):
        if self.hextext.isModified():
            try:
                self.slot_set_value(qcalc.mprealformatting.from_hex(self.hextext.text()))
            except:
                self.dectext.setText("")
                self.hextext.setText("")
                self.octtext.setText("")
                self.bintext.setText("")

    def slot_octal_editing_finished(self):
        if self.octtext.isModified():
            try:
                self.slot_set_value(qcalc.mprealformatting.from_octal(self.octtext.text()))
            except:
                self.dectext.setText("")
                self.hextext.setText("")
                self.octtext.setText("")
                self.bintext.setText("")

    def slot_binary_editing_finished(self):
        if self.bintext.isModified():
            try:
                self.slot_set_value(qcalc.mprealformatting.from_binary(self.bintext.text()))
            except:
                self.dectext.setText("")
                self.hextext.setText("")
                self.octtext.setText("")
                self.bintext.setText("")

    def slot_set_value(self, value):
        r = mpreal
        try:
            r = mpreal(value)
        except:
            try:
                r = qcalc.mprealformatting.from_decimal(value)
            except:
                self.dectext.setText("")
                self.hextext.setText("")
                self.octtext.setText("")
                self.bintext.setText("")
                return

        self.dectext.setText(qcalc.mprealformatting.to_decimal(r))
        if r.is_integer():
            self.hextext.setText(qcalc.mprealformatting.to_hex(r))
            self.octtext.setText(qcalc.mprealformatting.to_octal(r))
            self.bintext.setText(qcalc.mprealformatting.to_binary(r))
        else:
            self.hextext.setText("")
            self.octtext.setText("")
            self.bintext.setText("")

    def slot_expression_evaluated(self, expression, value):
        self.slot_set_value(value)

numWidget = NumeralSystemWidget
handler = KeyPadHandler
keyPadWidget = KeyPadWidget

containerWidget = QtWidgets.QWidget


def load():
    global numWidget
    global handler
    global keyPadWidget
    global containerWidget

    containerWidget = QtWidgets.QWidget(gui.wnd)

    numWidget = NumeralSystemWidget(containerWidget)
    handler = KeyPadHandler()
    keyPadWidget = KeyPadWidget(containerWidget)

    containerWidget.setLayout(QtWidgets.QHBoxLayout())
    containerWidget.layout().addWidget(numWidget, 1)
    containerWidget.layout().addWidget(keyPadWidget, 2)

    QtCore.QObject.connect(keyPadWidget, QtCore.SIGNAL("signal_key_pressed(int)"), handler,
                           QtCore.SLOT("slot_key_press(int)"))

    QtWidgets.QWidget.connect(gui.input_line_edit, QtCore.SIGNAL("textChanged(const QString &)"), numWidget,
                              QtCore.SLOT("slot_set_value(QString)"))
    QtWidgets.QWidget.connect(gui.wnd, QtCore.SIGNAL("signalExpressionEvaluated(const QString &, const QString &)"),
                              numWidget, QtCore.SLOT("slot_expression_evaluated(QString, QString)"))

    gui.root.layout().insertWidget(-1, containerWidget)


def unload():
    global numWidget
    global handler
    global keyPadWidget
    global containerWidget

    gui.root.layout().removeWidget(containerWidget)

    keyPadWidget.disconnect(handler)
    keyPadWidget.deleteLater()

    numWidget.disconnect(gui.input_line_edit)
    numWidget.disconnect(gui.wnd)
    numWidget.deleteLater()

    handler.deleteLater()
