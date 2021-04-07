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

root = find_widget("root_widget")
tabs = find_widget("tabWidget_main")
tab_history = find_widget("tab_history")
history = find_widget("widget_history")
input_line_edit = find_widget("lineEdit_input")
