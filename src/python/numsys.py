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
import qcalc.mprealformatting

from PySide2 import QtCore, QtWidgets


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

        spacer = QtWidgets.QSpacerItem(50, 50, QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Expanding)
        self.layout().addItem(spacer)

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
            except ValueError:
                self.slot_set_value(mpreal(0))

    def slot_hex_editing_finished(self):
        if self.hextext.isModified():
            try:
                self.slot_set_value(qcalc.mprealformatting.from_hex(self.hextext.text()))
            except ValueError:
                self.slot_set_value(mpreal(0))

    def slot_octal_editing_finished(self):
        if self.octtext.isModified():
            try:
                self.slot_set_value(qcalc.mprealformatting.from_octal(self.octtext.text()))
            except ValueError:
                self.slot_set_value(mpreal(0))

    def slot_binary_editing_finished(self):
        if self.bintext.isModified():
            try:
                self.slot_set_value(qcalc.mprealformatting.from_binary(self.bintext.text()))
            except ValueError:
                self.slot_set_value(mpreal(0))

    def slot_set_value(self, value):
        r = mpreal(value)
        self.dectext.setText(qcalc.mprealformatting.to_decimal(r))
        if r.is_integer():
            self.hextext.setText(qcalc.mprealformatting.to_hex(r))
            self.octtext.setText(qcalc.mprealformatting.to_octal(r))
            self.bintext.setText(qcalc.mprealformatting.to_binary(r))
        else:
            self.hextext.setText("")
            self.octtext.setText("")
            self.bintext.setText("")


widget = NumeralSystemWidget


def load():
    global widget
    widget = NumeralSystemWidget(gui.wnd)
    gui.tabs.addTab(widget, "Numeral Systems")


def unload():
    global widget
    gui.tabs.removeTab(gui.tabs.indexOf(widget))
    widget.deleteLater()
