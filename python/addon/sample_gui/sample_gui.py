"""
{
  "displayName": "Sample Addon - GUI",
  "description": "A sample addon demonstrating the use of the gui api."
}
"""

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

import qcalc as gui
import exprtk as exprtk

# We use PySide2 to create qt objects owned by python and integrate them into our native gui
# by using the QtWidgets.QApplication.instance() reference.
# If you think this is a hack have a look at:
# https://help.autodesk.com/view/3DSMAX/2017/ENU/?guid=__developer_using_pyside_html
# The gui module does the retrieval of the application instance as well as native gui elements for us.
from PySide2 import QtWidgets, QtCore

# Gui objects which should exist throughout the lifetime of the addon
# should be declared as global variables because otherwise the
# python owned object is destroyed while still being referenced from the native gui.
# The global scope of the addon module is retained for the lifetime of the application, because
# the interpreter is only started up and torn down with application start / exit.
menu = QtWidgets.QMenu
action = QtWidgets.QAction


# Our python side onclick callback
def onclick():
    print("Question Response: " + str(QtWidgets.QMessageBox.question(gui.wnd, "Title", "Text")))


# Load is invoked by the native code when the addon is requested to be loaded by the user.
def load():
    print("Loading gui sample addon")
    global menu
    global action

    # !IMPORTANT! Instantiate all new qt objects on the python side otherwise memory leaks will happen!
    menu = QtWidgets.QMenu()

    menu.setTitle("Sample Menu")
    gui.menubar.addMenu(menu)
    action = menu.addAction("Sample Action")

    # !IMPORTANT! Use QtCore.QObject.connect() to connect signals and slots, as the new way leaks memory.
    QtCore.QObject.connect(action, QtCore.SIGNAL('triggered()'), onclick)  # USE THIS
    # action.triggered.connect(onclick)  # NOT THIS, leaks memory.


# Unload is invoked by the native code when the addon is requested to be unloaded by the user
# It should perform cleanup such as removing gui elements.
# Note that the global scope of the module is currently still retained in memory after unload has been called because
# cpython does not offer a mechanism to unimport a module without tearing down the whole interpreter instance.
def unload():
    print("Unloading gui sample addon")
    global menu
    global action

    # !IMPORTANT! Always remove actions from the native QMenu by using the menu action, otherwise memory leaks!
    gui.menubar.removeAction(menu.menuAction())

    # !IMPORTANT! Call deleteLater() for qt objects created by python, otherwise memory leaks!
    menu.deleteLater()


# You can define logic at module level. This logic will be invoked when the module is imported.
# This happens once for the lifetime of the application when the module is requested to be loaded by the user
# and is immediately followed by a call to load().
print("Importing gui sample addon")
