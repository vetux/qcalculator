import qcalc.gui as gui
import qcalc.exprtk as exprtk

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


class SampleInputWidget(QtWidgets.QWidget):
    text = ""

    def input_changed(self, text):
        self.text = text

    def return_pressed(self):
        gui.input_line_edit.setText(str(exprtk.evaluate(self.text)))


sample_widget = SampleInputWidget


# Our python side onclick callback
def onclick():
    print("Question Response: " + str(QtWidgets.QMessageBox.question(gui.wnd, "Title", "Text")))


# Load is invoked by the native code when the addon is requested to be loaded by the user.
def load():
    print("Loading gui sample addon")
    global menu
    global action
    global sample_widget

    # !IMPORTANT! Instantiate all new qt objects on the python side otherwise memory leaks will happen!
    menu = QtWidgets.QMenu()

    menu.setTitle("Sample Menu")
    gui.menu.addMenu(menu)
    action = menu.addAction("Sample Action")

    # !IMPORTANT! Use QtCore.QObject.connect() to connect signals and slots, as the new way leaks memory.
    QtCore.QObject.connect(action, QtCore.SIGNAL('triggered()'), onclick)  # USE THIS
    # action.triggered.connect(onclick)  # NOT THIS, leaks memory.

    sample_widget = SampleInputWidget()

    # Store existing text in our sample widget
    sample_widget.text = gui.input_line_edit.text()

    # Disconnect the input field from the MainWindow
    gui.input_line_edit.disconnect(gui.wnd)

    # Register our own text changed and return pressed listener
    QtCore.QObject.connect(gui.input_line_edit,
                           QtCore.SIGNAL("returnPressed()"),
                           sample_widget,
                           QtCore.SLOT("return_pressed()"))

    QtCore.QObject.connect(gui.input_line_edit,
                           QtCore.SIGNAL("textChanged(QString)"),
                           sample_widget,
                           QtCore.SLOT("input_changed(QString)"))


# Unload is invoked by the native code when the addon is requested to be unloaded by the user
# It should perform cleanup such as removing gui elements.
# Note that the global scope of the module is currently still retained in memory after unload has been called because
# cpython does not offer a mechanism to unimport a module without tearing down the whole interpreter instance.
def unload():
    print("Unloading gui sample addon")
    global menu
    global action
    global sample_widget

    # !IMPORTANT! Always remove actions from the native QMenu by using the menu action, otherwise memory leaks!
    gui.menu.removeAction(menu.menuAction())

    # !IMPORTANT! Call deleteLater() for qt objects created by python, otherwise memory leaks!
    menu.deleteLater()

    # Disconnect our widget
    gui.input_line_edit.disconnect(sample_widget)

    # Reconnect the input field to the MainWindow
    QtCore.QObject.connect(gui.input_line_edit,
                           QtCore.SIGNAL("returnPressed()"),
                           gui.wnd,
                           QtCore.SLOT("onInputReturnPressed()"))
    QtCore.QObject.connect(gui.input_line_edit,
                           QtCore.SIGNAL("textChanged(QString)"),
                           gui.wnd,
                           QtCore.SLOT("onInputTextChanged(QString)"))

    # Return our text to the MainWindow
    gui.wnd.onInputTextChanged(sample_widget.text)

    sample_widget.deleteLater()


# You can define logic at module level. This logic will be invoked when the module is imported.
# This happens once for the lifetime of the application when the module is requested to be loaded by the user
# and is immediately followed by a call to load().
print("Importing gui sample addon")
