# The GUI module exposes the minimal native provided gui, which is only a main window instance which contains
# a menubar, a root widget with vertical layout and a line edit.

# All other gui is provided via python addons.

from PySide2 import QtWidgets


def widget(object_name):
    ws = widgets()
    for w in ws:
        if w.objectName() == object_name:
            return w


def widgets():
    global app
    return app.allWidgets()


app = QtWidgets.QApplication.instance()

wnd = widget("MainWindow")
menu = widget("menubar")
root = widget("rootWidget")
input_field = widget("lineEdit_input")
