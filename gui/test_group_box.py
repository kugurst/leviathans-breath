import editable_curve
import numpy as np
import pyqtgraph as pg

from PyQt5 import QtCore, QtGui, QtWidgets


# https://stackoverflow.com/questions/54760970/qt-layout-does-not-reflect-groupbox-size-change
class CollapsibleBox(QtWidgets.QGroupBox):
    def __init__(self, title="", parent=None):
        super(CollapsibleBox, self).__init__(title, parent)
        self.setCheckable(True)
        self.setChecked(False)
        self.toggled.connect(self.on_pressed)
        self.toggle_animation = QtCore.QParallelAnimationGroup(self)

        self.content_area = QtWidgets.QScrollArea(maximumHeight=0, minimumHeight=0)
        self.content_area.setSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Fixed)
        self.content_area.setFrameShape(QtWidgets.QFrame.NoFrame)

        lay = QtWidgets.QVBoxLayout(self)
        lay.setSpacing(0)
        lay.setContentsMargins(0, 0, 0, 0)
        lay.addWidget(self.content_area)

        self.toggle_animation.addAnimation(QtCore.QPropertyAnimation(self, b"minimumHeight"))
        self.toggle_animation.addAnimation(QtCore.QPropertyAnimation(self, b"maximumHeight"))
        self.toggle_animation.addAnimation(QtCore.QPropertyAnimation(self.content_area, b"maximumHeight"))

    @QtCore.pyqtSlot(bool)
    def on_pressed(self, checked):
        self.toggle_animation.setDirection(QtCore.QAbstractAnimation.Forward if checked else QtCore.QAbstractAnimation.Backward)
        self.toggle_animation.start()

    def setContentLayout(self, layout):
        lay = self.content_area.layout()
        del lay
        self.content_area.setLayout(layout)
        collapsed_height = self.sizeHint().height() - self.content_area.maximumHeight()
        content_height = layout.sizeHint().height()
        for i in range(self.toggle_animation.animationCount()):
            animation = self.toggle_animation.animationAt(i)
            animation.setDuration(500)
            animation.setStartValue(collapsed_height)
            animation.setEndValue(collapsed_height + content_height)

        content_animation = self.toggle_animation.animationAt(self.toggle_animation.animationCount() - 1)
        content_animation.setDuration(500)
        content_animation.setStartValue(0)
        content_animation.setEndValue(content_height)

    def make_curve(self):
        w = pg.PlotWidget(self)

        g = editable_curve.EditableCurve()
        w.addItem(g)
        w.getPlotItem().getViewBox().setMouseEnabled(x=False, y=False)

        x = np.linspace(1, 100, 40)
        pos = np.column_stack((x, np.sin(x)))

        g.setData(pos=pos, size=10, pxMode=True)

        return w, g


if __name__ == '__main__':
    import sys
    import random

    app = QtWidgets.QApplication(sys.argv)
    w = QtWidgets.QMainWindow()
    scroll = QtWidgets.QScrollArea()
    content = QtWidgets.QWidget()
    scroll.setWidget(content)
    scroll.setWidgetResizable(True)
    vlay = QtWidgets.QVBoxLayout(content)
    # counter = 0
    for i in range(10):
        box = CollapsibleBox("Collapsible Box Header-{}".format(i))
        widget, graph = box.make_curve()
        vlay.addWidget(box)
        lay = QtWidgets.QVBoxLayout()
        # for j in range(8):
        # btn = QtWidgets.QPushButton("PushButton-{}".format(counter))
        # print(widget.geometry())
        lay.addWidget(QtWidgets.QComboBox())
        lay.addWidget(widget)
        # counter += 1
        box.setContentLayout(lay)
    vlay.addStretch()
    w.setCentralWidget(scroll)
    w.resize(240, 480)
    w.show()

    sys.exit(app.exec_())