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
        self.ensure_monotonic_in_y = True
        self.ensure_first_and_last_y_same = False

        self.dragPoint = None
        self.drag_offset_x_ = None
        self.drag_offset_y_ = None

        self.min_x = 0
        self.max_x = 100
        self.min_y = 0
        self.max_y = 100

        self.data_update_callback = None
        self.data_drag_callback = None
        self.hover_point_callback = None

        pg.GraphItem.__init__(self)
        #
        # self.setGLOptions('translucent')

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
        old_points = self.data['pos']
        self.data['pos'] = points
        if len(points) != len(old_points):
            self.setData(**self.data)
        self.updateGraph()

    def updateGraph(self):
        pg.GraphItem.setData(self, **self.data)

    def mouseMoveEvent(self, ev):
        pos = self.getViewBox().mapSceneToView(ev)
        pts = self.scatter.pointsAt(pos)

        if pts and self.hover_point_callback:
            ind = pts[0].data()[0]
            self.hover_point_callback(ind)

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

            if self.data_update_callback:
                self.data_update_callback(ind)

            return
        else:
            super(EditableCurve, self).mousePressEvent(ev)

    def mouseDoubleClickEvent(self, ev: QtWidgets.QGraphicsSceneMouseEvent):
        if not self.add_new_point_on_double_click:
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
        self.ensure_monotonic_(insert_idx)
        if self.ensure_first_and_last_y_same and (insert_idx == len(self.data['pos']) or insert_idx == 0):
            self.align_first_and_last_y_(insert_idx)
        self.setData(**self.data)
        ev.accept()

        if self.data_update_callback:
            self.data_update_callback(insert_idx)

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
            if self.data_update_callback:
                self.data_update_callback(self.dragPoint.data()[0])
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

        self.ensure_monotonic_(ind)
        if self.ensure_first_and_last_y_same and (ind == len(self.data['pos']) - 1 or ind == 0):
            self.align_first_and_last_y_(ind)

        self.updateGraph()
        ev.accept()

        if self.data_drag_callback:
            self.data_drag_callback(ind)

    def align_first_and_last_y_(self, ind):
        points = self.data['pos']
        if ind == 0:
            points[-1][1] = points[0][1]
        elif ind == len(points) - 1:
            points[0][1] = points[-1][1]

    def ensure_monotonic_(self, modified_pos):
        points = self.data['pos']
        modified_point = points[modified_pos]

        # Fix the points less than
        for idx in range(0, modified_pos):
            point = points[idx]
            if point[0] > modified_point[0]:
                point[0] = modified_point[0]
            if self.ensure_monotonic_in_y:
                if point[1] > modified_point[1]:
                    point[1] = modified_point[1]

        # Fix the points greater than
        for idx in range(modified_pos + 1, len(points)):
            point = points[idx]
            if point[0] < modified_point[0]:
                point[0] = modified_point[0]
            if self.ensure_monotonic_in_y:
                if point[1] < modified_point[1]:
                    point[1] = modified_point[1]


class TimeSeriesCurve(EditableCurve):
    def __init__(self):
        super(self.__class__, self).__init__()

        self.add_new_point_on_double_click = False
        self.delete_point_on_right_click = False


class LEDCurveEditor(EditableCurve):
    def __init__(self):
        super(self.__class__, self).__init__()

        self.ensure_monotonic_in_y = False
        self.ensure_first_and_last_y_same = True


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

        self.view_x = [0, 100]
        self.view_y = [0, 100]

        self.widget.sceneObj.sigMouseMoved.connect(self.graph.mouseMoveEvent)
        self.widget.setBackground([255, 255, 255, 50])

    def set_view_range(self, x_range=None, y_range=None):
        new_x_range, new_y_range = None, None
        if x_range and len(y_range) == 2:
            if self.view_x != x_range:
                new_x_range = x_range
        if y_range and len(y_range) == 2:
            if self.view_y != y_range:
                new_y_range = y_range

        if new_x_range or new_y_range:
            self.widget.getPlotItem().getViewBox().disableAutoRange()
            self.widget.getPlotItem().getViewBox().setRange(xRange=new_x_range, yRange=new_y_range)


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
