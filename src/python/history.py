import qcalc.gui
import qcalc.numberformat

from PySide2 import QtCore, QtWidgets, QtGui


class HistoryItemWidget(QtWidgets.QWidget):
    layout = QtWidgets.QHBoxLayout
    expression_label = QtWidgets.QLabel
    equals_label = QtWidgets.QLabel
    result_label = QtWidgets.QLabel
    expression = ""
    result = ""

    def __init__(self, parent):
        QtWidgets.QWidget.__init__(self, parent)
        self.layout = QtWidgets.QHBoxLayout(self)
        self.expression_label = QtWidgets.QLabel(self)
        self.equals_label = QtWidgets.QLabel(self)
        self.result_label = QtWidgets.QLabel(self)

        self.expression_label.setTextInteractionFlags(QtCore.Qt.TextSelectableByMouse)
        self.expression_label.setSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Expanding)

        self.equals_label.setTextInteractionFlags(QtCore.Qt.TextSelectableByMouse)
        self.equals_label.setSizePolicy(QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Preferred)
        self.equals_label.setAlignment(QtCore.Qt.AlignCenter)

        self.result_label.setTextInteractionFlags(QtCore.Qt.TextSelectableByMouse)

        self.result_label.setAlignment(QtCore.Qt.AlignRight)
        self.result_label.setSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Expanding)

        self.layout.addWidget(self.expression_label, 6)
        self.layout.addWidget(self.equals_label, 1)
        self.layout.addWidget(self.result_label, 3)

        self.equals_label.setText("=")

    def resizeEvent(self, event):
        self.update_content()

    def set_content(self, expression, result):
        self.expression = expression
        self.result = result

    def update_content(self):
        self.expression_label.setText("")
        self.result_label.setText("")
        self.layout.activate()

        metrics = QtGui.QFontMetrics(self.expression_label.font())

        elided_text = metrics.elidedText(self.expression, QtCore.Qt.ElideRight, self.expression_label.width())
        self.expression_label.setText(elided_text)
        if self.expression != elided_text:
            self.expression_label.setToolTip(self.expression)
        else:
            self.expression_label.setToolTip("")

        elided_text = metrics.elidedText(self.result, QtCore.Qt.ElideRight, self.result_label.width())
        self.result_label.setText(elided_text)
        if self.result != elided_text:
            self.result_label.setToolTip(self.result)
        else:
            self.result_label.setToolTip("")


class HistoryListWidget(QtWidgets.QListWidget):
    history = []

    def __init__(self, parent):
        QtWidgets.QListWidget.__init__(self, parent)
        self.setVerticalScrollMode(QtWidgets.QAbstractItemView.ScrollPerPixel)

    def on_expression(self, expression, value):
        self.history.append((expression, value))
        self.update_list_widget()

    def update_list_widget(self):
        self.clear()
        for pair in self.history:
            widget = HistoryItemWidget(self)
            item = QtWidgets.QListWidgetItem(self)
            item.setSizeHint(widget.sizeHint())

            self.addItem(item)
            self.setItemWidget(item, widget)
            self.update()

            widget.set_content(pair[0], qcalc.numberformat.to_decimal(pair[1]))
        self.scrollToBottom()


history_widget = HistoryListWidget


def load():
    global history_widget
    history_widget = HistoryListWidget(qcalc.gui.wnd)
    qcalc.gui.root.layout().insertWidget(0, history_widget)
    QtCore.QObject.connect(qcalc.gui.wnd,
                           QtCore.SIGNAL("signalExpressionEvaluated(QString, double)"),
                           history_widget,
                           QtCore.SLOT("on_expression(QString, double)"))


def unload():
    global history_widget
    qcalc.gui.wnd.disconnect(history_widget)
    qcalc.gui.root.layout().removeWidget(history_widget)
    history_widget.deleteLater()
