import gui

import numpy as np
import pyqtgraph as pg
import qdarkstyle

from PyQt5 import QtCore, QtGui, QtWidgets

import editable_curve


class StyledGui(gui.Ui_mw_main):
    def __init__(self):
        super(self.__class__, self).__init__()
        self.rgb_channel_curves = []
        self.fan_curve = None

    def make_editable_curve(self):
        w = pg.PlotWidget(self)

        g = editable_curve.EditableCurve()
        w.addItem(g)
        w.getPlotItem().getViewBox().setMouseEnabled(x=False, y=False)

        # x = np.linspace(1, 100, 40)
        # pos = np.column_stack((x, np.sin(x)))

        # g.setData(pos=pos, size=10, pxMode=True)

        return w, g

    def init_editable_curves(self):
        self.fan_curve = editable_curve.EditableCurveCollection(*self.make_editable_curve())
        pass
