import constants
import editable_curve
import gui
import leviathans_breath
import math
import numpy as np
import os
import pyqtgraph as pg
import spinning_label_image

from PyQt5 import QtCore, QtGui, QtWidgets


class FanSpinningAnimation(object):
    def __init__(self, fan: spinning_label_image.SpinningLabelImage, name_button: QtWidgets.QPushButton,
                 rpm_label: QtWidgets.QLabel, v_p_label: QtWidgets.QLabel):
        self.fan = fan
        self.name_button = name_button
        self.rpm_label = rpm_label
        self.v_p_label = v_p_label


class GroupBoxAnimations(object):
    def __init__(self, open_anim: QtCore.QPropertyAnimation, close_anim: QtCore.QPropertyAnimation):
        self.open_anim = open_anim
        self.close_anim = close_anim


class StyledGui(gui.Ui_mw_main, QtWidgets.QMainWindow):
    def __init__(self):
        super(self.__class__, self).__init__()
        self.rgb_channel_curves = []  # type: list[editable_curve.EditableCurveCollection]
        self.rgb_preview_buttons = []  # type: list[QtWidgets.QPushButton]
        self.fan_widgets = []  # type: list[FanSpinningAnimation]
        self.fan_curve = None  # type: editable_curve.EditableCurveCollection
        self.temperature_series = None  # type: editable_curve.EditableCurveCollection
        self.parent = None
        self.rgb_tabs = []  # type: list[QtWidgets.QWidget]
        self.rgb_sync_combo_boxes = []  # type: list[QtWidgets.QComboBox]

        self.gb_anims = dict()  # type: dict[QtWidgets.QGroupBox, GroupBoxAnimations]

    def setupUi(self):
        super(self.__class__, self).setupUi(self)

        self.rgb_tabs = [self.tw_led_channel_t_r, self.tw_led_channel_t_g, self.tw_led_channel_t_b]
        self.rgb_sync_combo_boxes = [self.cb_r_channel_sync, self.cb_g_channel_sync, self.cb_b_channel_sync]

        self.init_curves()
        self.init_spinning_fans()
        self.init_led_previews()

        self.setAttribute(QtCore.Qt.WA_DontShowOnScreen)
        self.show()
        self.hide()
        self.setAttribute(QtCore.Qt.WA_DontShowOnScreen, False)

        sizes = self.sp_main.sizes()
        total_size = np.sum(sizes)
        left_size = math.floor(total_size * 5 / 7)
        right_size = total_size - left_size
        self.sp_main.setSizes([left_size, right_size])

        self.init_group_box_animations()

    def animate_gb_open_close(self, gb, checked):
        if gb not in self.gb_anims:
            return

        anims = self.gb_anims[gb]
        anim = anims.open_anim if checked else anims.close_anim
        anim.start()

    @QtCore.pyqtSlot(bool)
    def on_actionExit_triggered(self, state):
        self.close()

    @QtCore.pyqtSlot()
    def on_gb_temperature_display_clicked(self):
        self.animate_gb_open_close(self.gb_temperature_display, self.gb_temperature_display.isChecked())

    @QtCore.pyqtSlot()
    def on_gb_fan_curve_editor_clicked(self):
        self.animate_gb_open_close(self.gb_fan_curve_editor, self.gb_fan_curve_editor.isChecked())

    @QtCore.pyqtSlot()
    def on_gb_led_curve_options_clicked(self):
        self.animate_gb_open_close(self.gb_led_curve_options, self.gb_led_curve_options.isChecked())

    @QtCore.pyqtSlot()
    def on_gb_led_curve_editor_clicked(self):
        self.animate_gb_open_close(self.gb_led_curve_editor, self.gb_led_curve_editor.isChecked())

    @QtCore.pyqtSlot()
    def on_gb_fan_curve_options_clicked(self):
        self.animate_gb_open_close(self.gb_fan_curve_options, self.gb_fan_curve_options.isChecked())

    @QtCore.pyqtSlot()
    def on_pb_led_options_save_clicked(self):
        print("led saved")

    def build_spinning_fan(self, parent=None):
        if parent is None:
            parent = self

        label = spinning_label_image.SpinningLabelImage(parent, alignment=QtCore.Qt.AlignCenter)
        img_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "fan.svg")
        pixmap = QtGui.QPixmap(img_path)
        pixmap = pixmap.scaledToHeight(80, QtCore.Qt.SmoothTransformation)
        label.set_pixmap(pixmap)

        return label

    def make_curve(self, graph_type=editable_curve.EditableCurve):
        w = pg.PlotWidget(self.parent)

        g = graph_type()
        w.addItem(g)
        w.getPlotItem().getViewBox().setMouseEnabled(x=False, y=False)
        x_axis = w.getPlotItem().getAxis("bottom")  # type: pg.AxisItem
        y_axis = w.getPlotItem().getAxis("left")  # type: pg.AxisItem
        x_axis.setGrid(0)

        x_axis.setStyle(stopAxisAtTick=(False, True))
        y_axis.setStyle(stopAxisAtTick=(True, True))
        x_axis.setPen(color=(255, 255, 255))
        y_axis.setPen(color=(255, 255, 255))
        # x_axis.setLabel()
        # x_axis.update()

        return w, g

    def init_curves(self):
        self.fan_curve = editable_curve.EditableCurveCollection(*self.make_curve())
        for _ in range(leviathans_breath.NUM_LED_CHANNELS()):
            self.rgb_channel_curves.append(
                editable_curve.EditableCurveCollection(*self.make_curve(graph_type=editable_curve.LEDCurveEditor)))
        self.temperature_series = editable_curve.EditableCurveCollection(
            *self.make_curve(graph_type=editable_curve.TimeSeriesCurve))

        self.gl_fan_curve_ph.addWidget(self.fan_curve.widget)
        self.gl_tw_led_channel_t_r_ph.addWidget(self.rgb_channel_curves[0].widget)
        self.gl_tw_led_channel_t_g_ph.addWidget(self.rgb_channel_curves[1].widget)
        self.gl_tw_led_channel_t_b_ph.addWidget(self.rgb_channel_curves[2].widget)
        self.gl_temperature_display_ph.addWidget(self.temperature_series.widget)

        self.temperature_series.graph.setPen(width=5)
        self.fan_curve.graph.setPen(width=5)
        for curve in self.rgb_channel_curves:
            curve.graph.setPen(width=5)

    def init_spinning_fans(self):
        for idx in range(leviathans_breath.NUM_FANS()):
            fan_info_frame = QtWidgets.QFrame(self)
            fan_info_layout = QtWidgets.QVBoxLayout()
            fan_info_frame.setLayout(fan_info_layout)

            pb = QtWidgets.QPushButton("", self)
            pb.setFlat(True)
            pb.setProperty("class_", "led_preview_button")
            fan_info_layout.addWidget(pb)  # , 0, 0)

            spinning_fan_frame = QtWidgets.QFrame(self)
            fan_info_layout.addWidget(spinning_fan_frame)  # , 1, 0)
            layout = self.sawc_fan_status.layout()  # type: QtWidgets.QGridLayout
            layout.addWidget(fan_info_frame, math.floor(idx / 2), idx % 2)

            spinning_fan = self.build_spinning_fan(spinning_fan_frame)
            spinning_fan.start_animation()

            spinning_fan_frame.setFixedHeight(spinning_fan.pixmap().height())
            spinning_fan_frame.setFixedWidth(spinning_fan.pixmap().height())
            spinning_fan_frame.setProperty("faux-class", "img_frame")
            # spinning_fan_frame.setStyleSheet("background-color: rgba(255, 255, 255, 0)")
            fan_info_layout.setAlignment(spinning_fan_frame, QtCore.Qt.AlignHCenter)

            rpm_label = QtWidgets.QLabel("2600 RPM")
            fan_info_layout.addWidget(rpm_label)  # , 2, 0)
            fan_info_layout.setAlignment(rpm_label, QtCore.Qt.AlignHCenter)

            v_p_label = QtWidgets.QLabel("100%")
            fan_info_layout.addWidget(v_p_label)
            fan_info_layout.setAlignment(v_p_label, QtCore.Qt.AlignHCenter)

            fan_info_frame.setProperty("class_", "fan_container")
            fan_info_frame.setStyleSheet("border-radius: 0")
            spinning_fan_frame.setProperty("class_", "fan_box")

            self.fan_widgets.append(FanSpinningAnimation(spinning_fan, pb, rpm_label, v_p_label))

    def init_led_previews(self):
        for idx in range(leviathans_breath.NUM_LEDS()):
            pb = QtWidgets.QPushButton("", self)
            pb.setFlat(True)
            pb.setProperty("faux-class", "led-button")
            self.gl_led_preview.addWidget(pb, idx, 0)
            self.rgb_preview_buttons.append(pb)

    def make_group_box_animations(self, widget: QtWidgets.QWidget):
        size_hint = widget.sizeHint()

        close_anim = QtCore.QVariantAnimation(widget)
        close_anim.setStartValue(size_hint.height())
        close_anim.setEndValue(0)
        close_anim.setEasingCurve(QtCore.QEasingCurve.InOutQuart)
        close_anim.valueChanged.connect(lambda val: widget.setFixedHeight(val))
        close_anim.setDuration(250)

        open_anim = QtCore.QVariantAnimation(widget)
        open_anim.setStartValue(0)
        open_anim.setEndValue(size_hint.height())
        open_anim.setEasingCurve(QtCore.QEasingCurve.InOutQuart)
        open_anim.valueChanged.connect(lambda val: widget.setFixedHeight(val))
        open_anim.setDuration(250)

        return open_anim, close_anim

    def init_group_box_animations(self):
        for gb in [self.gb_temperature_display, self.gb_fan_curve_editor, self.gb_fan_curve_options,
                   self.gb_led_curve_editor, self.gb_led_curve_options]:
            self.gb_anims[gb] = GroupBoxAnimations(*self.make_group_box_animations(gb.children()[1]))

        self.gb_led_curve_options.setChecked(False)
        self.on_gb_led_curve_options_clicked()
        self.gb_fan_curve_options.setChecked(False)
        self.on_gb_fan_curve_options_clicked()


# https://stackoverflow.com/questions/55977559/changing-qgroupbox-checkbox-visual-to-an-expander
class GroupBoxProxyStyle(QtWidgets.QProxyStyle):
    def subControlRect(self, control, option, subControl, widget):
        ret = super(GroupBoxProxyStyle, self).subControlRect(
            control, option, subControl, widget
        )
        if (
                control == QtWidgets.QStyle.CC_GroupBox
                and subControl == QtWidgets.QStyle.SC_GroupBoxLabel
                and widget.isCheckable()
        ):
            r = self.subControlRect(
                QtWidgets.QStyle.CC_GroupBox,
                option,
                QtWidgets.QStyle.SC_GroupBoxCheckBox,
                widget,
            )
            ret.adjust(r.width(), 0, 0, 0)
        return ret

    def drawComplexControl(self, control, option, painter, widget):
        is_group_box = False
        if control == QtWidgets.QStyle.CC_GroupBox and widget.isCheckable():
            option.subControls &= ~QtWidgets.QStyle.SC_GroupBoxCheckBox
            is_group_box = True
        super(GroupBoxProxyStyle, self).drawComplexControl(
            control, option, painter, widget
        )
        if is_group_box and widget.isCheckable():
            opt = QtWidgets.QStyleOptionViewItem()
            opt.rect = self.proxy().subControlRect(
                QtWidgets.QStyle.CC_GroupBox,
                option,
                QtWidgets.QStyle.SC_GroupBoxCheckBox,
                widget,
            )
            opt.state = QtWidgets.QStyle.State_Children
            opt.state |= (
                QtWidgets.QStyle.State_Open
                if widget.isChecked()
                else QtWidgets.QStyle.State_None
            )
            self.drawPrimitive(
                QtWidgets.QStyle.PE_IndicatorBranch, opt, painter, widget
            )
