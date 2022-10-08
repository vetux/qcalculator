"""
{
  "displayName": "Number View",
  "description": "This addon adds gui for converting values between various numeral systems."
}
"""

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

import qcalc as gui
import decimal

from PySide2 import QtCore, QtWidgets

class NumeralSystemWidget(QtWidgets.QWidget):
    def __init__(self, parent):
        QtWidgets.QWidget.__init__(self, parent)
        self.setLayout(QtWidgets.QVBoxLayout())

        self.layout().setMargin(0)

        hexlayout = QtWidgets.QHBoxLayout()
        label = QtWidgets.QLabel(self)
        label.setText("Hex")
        label.setSizePolicy(QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Preferred))
        label.setAlignment(QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter)
        label.setMinimumSize(70, 0)
        self.hextext = QtWidgets.QLineEdit(self)
        hexlayout.addWidget(self.hextext, 1)
        hexlayout.addWidget(label, 0)

        octlayout = QtWidgets.QHBoxLayout()
        label = QtWidgets.QLabel(self)
        label.setText("Octal")
        label.setSizePolicy(QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Preferred))
        label.setAlignment(QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter)
        label.setMinimumSize(70, 0)
        self.octtext = QtWidgets.QLineEdit(self)
        octlayout.addWidget(self.octtext, 1)
        octlayout.addWidget(label, 0)

        binlayout = QtWidgets.QHBoxLayout()
        label = QtWidgets.QLabel(self)
        label.setText("Binary")
        label.setSizePolicy(QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Preferred))
        label.setAlignment(QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter)
        label.setMinimumSize(70, 0)
        self.bintext = QtWidgets.QLineEdit(self)
        binlayout.addWidget(self.bintext, 1)
        binlayout.addWidget(label, 0)

        scilayout = QtWidgets.QHBoxLayout()
        label = QtWidgets.QLabel(self)
        label.setText("Scientific")
        label.setSizePolicy(QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Preferred))
        label.setAlignment(QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter)
        label.setMinimumSize(70, 0)
        self.scitext = QtWidgets.QLineEdit(self)
        scilayout.addWidget(self.scitext, 1)
        scilayout.addWidget(label, 0)

        unilayout = QtWidgets.QHBoxLayout()
        label = QtWidgets.QLabel(self)
        label.setText("Unicode")
        label.setSizePolicy(QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Preferred))
        label.setAlignment(QtCore.Qt.AlignLeft | QtCore.Qt.AlignVCenter)
        label.setMinimumSize(70, 0)
        self.unitext = QtWidgets.QLineEdit(self)
        unilayout.addWidget(self.unitext, 1)
        unilayout.addWidget(label, 0)

        self.layout().addLayout(scilayout)
        self.layout().addLayout(hexlayout)
        self.layout().addLayout(octlayout)
        self.layout().addLayout(binlayout)
        self.layout().addLayout(unilayout)

        QtCore.QObject.connect(self.scitext,
                               QtCore.SIGNAL("textChanged(const QString &)"),
                               self,
                               QtCore.SLOT("slot_scientific_editing_finished()"))
        QtCore.QObject.connect(self.unitext,
                               QtCore.SIGNAL("textChanged(const QString &)"),
                               self,
                               QtCore.SLOT("slot_unicode_editing_finished()"))
        QtCore.QObject.connect(self.hextext,
                               QtCore.SIGNAL("textChanged(const QString &)"),
                               self,
                               QtCore.SLOT("slot_hex_editing_finished()"))
        QtCore.QObject.connect(self.octtext,
                               QtCore.SIGNAL("textChanged(const QString &)"),
                               self,
                               QtCore.SLOT("slot_octal_editing_finished()"))
        QtCore.QObject.connect(self.bintext,
                               QtCore.SIGNAL("textChanged(const QString &)"),
                               self,
                               QtCore.SLOT("slot_binary_editing_finished()"))

    def slot_scientific_editing_finished(self):
        if self.scitext.isModified():
            try:
                self.slot_set_value(decimal.Decimal(str(decimal.Decimal(self.scitext.text()))))
            except Exception as e:
                print(str(e))
                self.unitext.setText("")
                self.hextext.setText("")
                self.octtext.setText("")
                self.bintext.setText("")

    def slot_unicode_editing_finished(self):
        if self.unitext.isModified():
            try:
                self.slot_set_value(ord(self.unitext.text()[0]))
            except Exception as e:
                print(str(e))
                self.scitext.setText("")
                self.hextext.setText("")
                self.octtext.setText("")
                self.bintext.setText("")

    def slot_hex_editing_finished(self):
        if self.hextext.isModified():
            try:
                self.slot_set_value(decimal.Decimal(int(self.hextext.text(), 16)))
            except Exception as e:
                print(str(e))
                self.scitext.setText("")
                self.unitext.setText("")
                self.octtext.setText("")
                self.bintext.setText("")

    def slot_octal_editing_finished(self):
        if self.octtext.isModified():
            try:
                self.slot_set_value(decimal.Decimal(int(self.octtext.text(), 8)))
            except Exception as e:
                print(str(e))
                self.scitext.setText("")
                self.unitext.setText("")
                self.hextext.setText("")
                self.bintext.setText("")

    def slot_binary_editing_finished(self):
        if self.bintext.isModified():
            try:
                self.slot_set_value(decimal.Decimal(int(self.bintext.text(), 2)))
            except Exception as e:
                print(str(e))
                self.scitext.setText("")
                self.unitext.setText("")
                self.hextext.setText("")
                self.octtext.setText("")

    def slot_set_value(self, value, signal = True):
        r = decimal.Decimal(0)
        try:
            r = decimal.Decimal(value)
        except Exception as e:
            print(str(e))
            if not signal:
                self.scitext.setText("")
                self.unitext.setText("")
                self.hextext.setText("")
                self.octtext.setText("")
                self.bintext.setText("")
            return

        if signal:
            self.signal_value_changed.emit(format(r, 'f'))

        self.scitext.setText('%E' % r)

        ri = r.to_integral_value()

        if r.is_finite() and r == ri:
            value = int(format(ri, 'f'))
            if value < 0x10FFFF and value > 0:
                self.unitext.setText(chr(value))
            else:
                self.unitext.setText("")
            self.hextext.setText(hex(value))
            self.octtext.setText(oct(value))
            self.bintext.setText(bin(value))
        else:
            self.unitext.setText("")
            self.hextext.setText("")
            self.octtext.setText("")
            self.bintext.setText("")

    def slot_expression_evaluated(self, expression, value):
        self.slot_set_value(value, False)

    def slot_input_text_changed(self, text):
        self.slot_set_value(text, False)

    signal_value_changed = QtCore.Signal("QString")

numWidget = NumeralSystemWidget

containerWidget = QtWidgets.QWidget

def load():
    global numWidget
    global containerWidget

    containerWidget = QtWidgets.QWidget(gui.wnd)
    numWidget = NumeralSystemWidget(containerWidget)

    containerWidget.setLayout(QtWidgets.QHBoxLayout())
    containerWidget.layout().addWidget(numWidget, 1)
    containerWidget.layout().setMargin(0)

    gui.insert_widget_footer(containerWidget, 0, 0)

    QtWidgets.QWidget.connect(gui.input_line_edit,
                              QtCore.SIGNAL("textChanged(const QString &)"),
                              numWidget,
                              QtCore.SLOT("slot_input_text_changed(QString)"))
    QtWidgets.QWidget.connect(gui.wnd,
                              QtCore.SIGNAL("signalExpressionEvaluated(const QString &, const QString &)"),
                              numWidget,
                              QtCore.SLOT("slot_expression_evaluated(QString, QString)"))
    QtWidgets.QWidget.connect(numWidget,
                              QtCore.SIGNAL("signal_value_changed(QString)"),
                              gui.input_line_edit,
                              QtCore.SLOT("setText(const QString &)"))

    numWidget.slot_input_text_changed(gui.input_line_edit.text())

def unload():
    gui.remove_widget_footer(containerWidget)

    numWidget.disconnect(gui.input_line_edit)
    numWidget.disconnect(gui.wnd)
    numWidget.deleteLater()

    containerWidget.deleteLater()
