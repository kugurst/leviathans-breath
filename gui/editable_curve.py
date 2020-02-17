import sys
import numpy as np
import pyqtgraph as pg
from pyqtgraph.Qt import QtCore, QtGui


class EditableCurve(pg.GraphItem):
    def __init__(self):
        self.dragPoint = None
        self.drag_offset_y_ = None
        self.drag_offset_x_ = None

        self.min_x = 0
        self.max_x = 100
        self.min_y = 0
        self.max_y = 100

        self.callback = None

        pg.GraphItem.__init__(self)

    def setData(self, **kwds):
        self.data = kwds
        if 'pos' in self.data:
            npts = self.data['pos'].shape[0]
            self.data['adj'] = np.column_stack((np.arange(0, npts - 1), np.arange(1, npts)))
            self.data['data'] = np.empty(npts, dtype=[('index', int)])
            self.data['data']['index'] = np.arange(npts)
        self.updateGraph()

    def updateGraph(self):
        pg.GraphItem.setData(self, **self.data)

    def mouseDragEvent(self, ev):
        if ev.button() != QtCore.Qt.LeftButton:
            ev.ignore()
            return

        if ev.isStart():
            pos = ev.buttonDownPos()
            pts = self.scatter.pointsAt(pos)
            if len(pts) == 0:
                ev.ignore()
                return
            self.dragPoint = pts[0]
            ind = pts[0].data()[0]
            self.drag_offset_y_ = self.data['pos'][ind][1] - pos[1]
            self.drag_offset_x_ = self.data['pos'][ind][0] - pos[0]
        elif ev.isFinish():
            if self.callback:
                self.callback(self.dragPoint.data()[0])
            self.dragPoint = None
            return
        else:
            if self.dragPoint is None:
                ev.ignore()
                return

        ind = self.dragPoint.data()[0]
        self.data['pos'][ind][1] = ev.pos()[1] + self.drag_offset_y_
        self.data['pos'][ind][0] = ev.pos()[0] + self.drag_offset_x_

        self.updateGraph()
        ev.accept()


class EditableCurveCollection(object):
    def __init__(self, widget: pg.PlotWidget, graph: EditableCurve):
        self.widget = widget
        self.graph = graph


def main():
    pg.setConfigOptions(antialias=True)

    w = pg.GraphicsWindow()
    w.setWindowTitle('Draggable')

    g = EditableCurve()
    v = w.addPlot()
    v.addItem(g)

    x = np.linspace(1, 100, 40)
    pos = np.column_stack((x, np.sin(x)))

    g.setData(pos=pos, size=10, pxMode=True)

    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtGui.QApplication.instance().exec_()


if __name__ == '__main__':
    main()
