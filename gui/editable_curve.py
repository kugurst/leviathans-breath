import constants
import math
import numpy as np
import pyqtgraph as pg
import sys
from pyqtgraph.Qt import QtCore, QtGui, QtWidgets


class EditableCurve(pg.GraphItem):
    def __init__(self):
        self.add_new_point_on_double_click = True
        self.delete_point_on_right_click = True

        self.dragPoint = None
        self.drag_offset_x_ = None
        self.drag_offset_y_ = None

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

    def get_points(self) -> np.ndarray:
        return self.data['pos']

    def set_points(self, points: np.ndarray):
        self.data['pos'] = points
        self.updateGraph()

    def updateGraph(self):
        pg.GraphItem.setData(self, **self.data)

    def mousePressEvent(self, ev: QtWidgets.QGraphicsSceneMouseEvent):
        if not self.delete_point_on_right_click:
            super(EditableCurve, self).mousePressEvent(ev)
            return

        pos = ev.pos()  # type: QtCore.QPointF
        pts = self.scatter.pointsAt(pos)
        points = self.data['pos']

        if len(pts) > 0 and (ev.buttons() & QtCore.Qt.RightButton) and len(points) > 2:
            self.dragPoint = pts[0]
            ind = pts[0].data()[0]
            self.data['pos'] = np.delete(points, ind, axis=0)
            self.setData(**self.data)
            ev.accept()

            if self.callback:
                self.callback(ind)

            return
        else:
            super(self.__class__, self).mousePressEvent(ev)

    def mouseDoubleClickEvent(self, ev: QtWidgets.QGraphicsSceneMouseEvent):
        if not self.add_new_point_on_double_click:
            print("ignoring double click")
            ev.ignore()
            return

        pos = ev.pos()  # type: QtCore.QPointF
        pts = self.scatter.pointsAt(pos)

        if len(pts):
            ev.ignore()
            return
        if pos.x() < self.min_x or pos.x() > self.max_x or pos.y() < self.min_y or pos.y() > self.max_y:
            ev.ignore()
            return

        rounded_x, rounded_y = quarter_round(pos.x()), quarter_round(pos.y())

        points = self.data['pos']
        insert_idx = len(points)
        for idx, point in enumerate(points):
            if rounded_x < point[0]:
                insert_idx = idx
                break

        self.data['pos'] = np.insert(points, insert_idx, [rounded_x, rounded_y], axis=0)
        self.setData(**self.data)
        ev.accept()

        if self.callback:
            self.callback(insert_idx)

        return

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
            self.drag_offset_x_ = self.data['pos'][ind][0] - pos[0]
            self.drag_offset_y_ = self.data['pos'][ind][1] - pos[1]
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
        new_x = ev.pos()[0] + self.drag_offset_x_
        new_y = ev.pos()[1] + self.drag_offset_y_

        self.data['pos'][ind][0] = quarter_round(min(self.max_x, max(self.min_x, new_x)))
        self.data['pos'][ind][1] = quarter_round(min(self.max_y, max(self.min_y, new_y)))

        # print("x:", self.data['pos'][ind][0])
        # print("y:", self.data['pos'][ind][1])

        self.updateGraph()
        ev.accept()


class TimeSeriesCurve(EditableCurve):
    def __init__(self):
        super(self.__class__, self).__init__()

        self.add_new_point_on_double_click = False
        self.delete_point_on_right_click = False


def search(arr, x):
    if x < arr[0][0]:
        return arr[0], arr[0]
    if x > arr[-1][0]:
        return arr[-1], arr[-1]

    lo = 0
    hi = len(arr) - 1

    while lo <= hi:
        mid = math.floor((hi + lo) / 2)

        if x < arr[mid][0]:
            hi = mid - 1
        elif x > arr[mid][0]:
            lo = mid + 1
        else:
            return arr[mid], arr[mid]

    return arr[hi], arr[lo]


def quarter_round(x):
    return np.round(x * 4) / 4


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
