# QCalc - Extensible programming calculator
# Copyright (C) 2022  Julian Zampiccoli
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

from PySide2 import QtWidgets

def find_widget(object_name):
    ws = app.allWidgets()
    for w in ws:
        if w.objectName() == object_name:
            return w

app = QtWidgets.QApplication.instance()

wnd = find_widget("MainWindow")
menubar = find_widget("menubar")
statusbar = find_widget("statusbar")

root = find_widget("widget_root")
history = find_widget("widget_history")
input_line_edit = find_widget("lineEdit_input")

footer_widget = find_widget("footerWidget")

footer_rows = {}
widget_row_mapping = {}

def insert_widget_footer(widget, column, row):
    global footer_widget
    global footer_rows
    global widget_row_mapping

    if not row in footer_rows:
        row_widget = QtWidgets.QWidget()
        row_widget.setLayout(QtWidgets.QHBoxLayout()) # The layout that contains the row widgets
        row_widget.layout().setMargin(0)
        footer_rows[row] = row_widget
        footer_widget.layout().insertWidget(row, row_widget)
    else:
        row_widget = footer_rows[row]

    row_widget.layout().insertWidget(column, widget)

    widget_row_mapping[widget] = row

    footer_widget.show()

def remove_widget_footer(widget):
    global widget_row_mapping

    row = widget_row_mapping[widget]
    row_widget = footer_rows[row]

    row_widget.layout().removeWidget(widget)

    if row_widget.layout().count() == 0:
        footer_widget.layout().removeWidget(row_widget)
        del footer_rows[row]

    del widget_row_mapping[widget]

    if len(widget_row_mapping) == 0:
        footer_widget.hide()