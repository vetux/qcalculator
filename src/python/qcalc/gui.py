# Instead of using shiboken we will interact with the gui through QtWidgets.QApplication.instance()
# and expose other functionality by hand.

# All native provided gui should be referenced through the global variables defined in this module
# as their object names may change.


from PySide2 import QtWidgets


def widget(objectname):
    ws = widgets()
    for w in ws:
        if w.objectName() == objectname:
            return w


def widgets():
    global app
    return app.allWidgets()


app = QtWidgets.QApplication.instance()
wnd = widget("MainWindow")
menu = widget("menubar")
