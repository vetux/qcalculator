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

# The GUI module exposes the minimal native provided gui to the addons.
from PySide2 import QtWidgets


def find_widget(object_name):
    ws = app.allWidgets()
    for w in ws:
        if w.objectName() == object_name:
            return w


app = QtWidgets.QApplication.instance()

wnd = find_widget("MainWindow")
menu = find_widget("menubar")
statusbar = find_widget("statusbar")

menu_file = find_widget("menuFile")
menu_help = find_widget("menuHelp")

action_settings = find_widget("actionSettings")
action_exit = find_widget("actionExit")
action_about = find_widget("actionAbout")

root = find_widget("widget_root")
tabs = find_widget("tabWidget_main")
history = find_widget("widget_history")
input_line_edit = find_widget("lineEdit_input")
