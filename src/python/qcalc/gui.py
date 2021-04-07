# The GUI module exposes the minimal native provided gui to the addons.


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
statusbar = widget("statusbar")

menu_file = widget("menuFile")
menu_help = widget("menuHelp")

action_settings = widget("actionSettings")
action_exit = widget("actionExit")
action_about = widget("actionAbout")

root = widget("root_widget")
tabs = widget("tabWidget_main")
tab_history = widget("tab_history")
history = widget("widget_history")
input_line_edit = widget("lineEdit_input")
