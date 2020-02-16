import test_group_box as collapsible_box
import editable_curve
import gui
import leviathans_breath
import numpy as np
import pyqtgraph as pg

from PyQt5 import QtCore, QtGui, QtWidgets, Qt


class GroupBoxAnimations(object):
    def __init__(self, open_anim: QtCore.QPropertyAnimation, close_anim: QtCore.QPropertyAnimation):
        self.open_anim = open_anim
        self.close_anim = close_anim


class StyledGui(gui.Ui_mw_main, QtGui.QMainWindow):
    def __init__(self):
        super(self.__class__, self).__init__()
        self.rgb_channel_curves = []  # type: list[editable_curve.EditableCurveCollection]
        self.fan_curve = None
        self.temperature_series = None
        self.parent = None

        self.gb_anims = dict()  # type: dict[QtWidgets.QGroupBox, GroupBoxAnimations]

    def setupUi(self):
        super(self.__class__, self).setupUi(self)

        self.init_curves()

        self.setAttribute(QtCore.Qt.WA_DontShowOnScreen)
        self.show()
        self.hide()
        self.setAttribute(QtCore.Qt.WA_DontShowOnScreen, False)

        self.init_group_box_animations()

    def animate_gb_close(self, gb, checked):
        if gb not in self.gb_anims:
            return

        print(gb.children()[1].height())

        anims = self.gb_anims[gb]
        anim = anims.open_anim if checked else anims.close_anim
        anim.start()
        gb.updateGeometry()
        pass

    @QtCore.pyqtSlot()
    def on_gb_temperature_display_clicked(self):
        self.animate_gb_close(self.gb_temperature_display, self.gb_temperature_display.isChecked())

    @QtCore.pyqtSlot()
    def on_gb_curve_editor_clicked(self):
        self.animate_gb_close(self.gb_curve_editor, self.gb_curve_editor.isChecked())

    @QtCore.pyqtSlot()
    def on_gb_led_curve_options_clicked(self):
        self.animate_gb_close(self.gb_led_curve_options, self.gb_led_curve_options.isChecked())

    @QtCore.pyqtSlot()
    def on_gb_led_curve_editor_clicked(self):
        self.animate_gb_close(self.gb_led_curve_editor, self.gb_led_curve_editor.isChecked())

    @QtCore.pyqtSlot()
    def on_gb_fan_curve_options_clicked(self):
        self.animate_gb_close(self.gb_fan_curve_options, self.gb_fan_curve_options.isChecked())

    @QtCore.pyqtSlot()
    def on_pb_led_options_save_clicked(self):
        print("led saved")

    def make_curve(self):
        w = pg.PlotWidget(self.parent)

        g = editable_curve.EditableCurve()
        w.addItem(g)
        w.getPlotItem().getViewBox().setMouseEnabled(x=False, y=False)

        # x = np.linspace(1, 100, 40)
        # pos = np.column_stack((x, np.sin(x)))
        #
        # g.setData(pos=pos, size=10, pxMode=True)

        return w, g

    def init_curves(self):
        self.fan_curve = editable_curve.EditableCurveCollection(*self.make_curve())
        for _ in range(leviathans_breath.NUM_LED_CHANNELS()):
            self.rgb_channel_curves.append(editable_curve.EditableCurveCollection(*self.make_curve()))
        self.temperature_series = editable_curve.EditableCurveCollection(*self.make_curve())

        self.gl_fan_curve_ph.addWidget(self.fan_curve.widget)
        self.gl_tw_led_channel_t_r_ph.addWidget(self.rgb_channel_curves[0].widget)
        self.gl_tw_led_channel_t_g_ph.addWidget(self.rgb_channel_curves[1].widget)
        self.gl_tw_led_channel_t_b_ph.addWidget(self.rgb_channel_curves[2].widget)
        self.gl_temperature_display_ph.addWidget(self.temperature_series.widget)

    def make_group_box_animations(self, widget: QtWidgets.QWidget):
        size_hint = widget.sizeHint()
        default_geom = widget.geometry()  # type: QtCore.QRect
        default_geom.setHeight(size_hint.height())
        closed_geom = QtCore.QRect(default_geom)
        closed_geom.setHeight(0)

        close_anim = QtCore.QPropertyAnimation(widget, b"geometry")
        close_anim.setStartValue(default_geom)
        close_anim.setEndValue(closed_geom)
        close_anim.setEasingCurve(QtCore.QEasingCurve.OutBounce)

        open_anim = QtCore.QPropertyAnimation(widget, b"geometry")
        open_anim.setStartValue(closed_geom)
        open_anim.setEndValue(default_geom)
        open_anim.setEasingCurve(QtCore.QEasingCurve.OutBounce)

        return open_anim, close_anim

    def init_group_box_animations(self):
        for gb in [self.gb_temperature_display, self.gb_curve_editor, self.gb_fan_curve_options,
                   self.gb_led_curve_editor, self.gb_led_curve_options]:
            self.gb_anims[gb] = GroupBoxAnimations(*self.make_group_box_animations(gb.children()[1]))
        pass
