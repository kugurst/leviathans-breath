import enum
import os

import constants
import db_config
import driver_process
import editable_curve
import json
import leviathans_breath
import numpy as np
import styled_gui
import yaml

from PyQt5 import QtCore, QtGui, QtWidgets

_MAX_TIME_STEP = 100


@enum.unique
class SyncConfig(enum.IntEnum):
    SELF_SYNC = db_config.SerializableClass.INVALID_INDEX
    R_CHAN = 0
    G_CHAN = 1
    B_CHAN = 2


class StyledGuiController(QtWidgets.QWidget):
    POINTS_JSON_KEY = "points"
    RGB_CHAN_JSON_KEYS = ["r", "g", "b"]
    FAN_PARAMS_JSON_KEY = "fans"
    RPM_JSON_KEY = "rpms"
    LED_PARAMS_JSON_KEY = "leds"
    TEMPERATURES_JSON_KEY = "temperatures"

    def __init__(self, gui: styled_gui.StyledGui):
        super(self.__class__, self).__init__()
        self.gui = gui
        self.db = db_config.DB.build_default()

        if os.path.isfile(constants.DB_FILE_NAME):
            try:
                self.db.load(constants.DB_FILE_NAME)
            except yaml.YAMLError:
                os.unlink(constants.DB_FILE_NAME)

        self.spinner_timer = QtCore.QTimer(self)
        self.spinner_timer.timeout.connect(self.set_speed_multiplier_)
        self.spinner_timer.setSingleShot(True)

        self.led_timer = QtCore.QTimer(self)
        self.led_timer.timeout.connect(self._update_led_display)
        self.led_timer.start(round(1 / self.db.gui_config.led_update_rate * 1000))
        self.time_step = [0] * leviathans_breath.NUM_LEDS()

        self.temperature_timer = QtCore.QTimer(self)
        self.temperature_timer.timeout.connect(self._update_temperature_graph)
        self.temperature_timer.start(round(1 / self.db.gui_config.temp_update_rate * 1000))

        self.fan_timer = QtCore.QTimer(self)
        self.fan_timer.timeout.connect(self._update_fan_status)
        self.fan_timer.start(round(1 / self.db.gui_config.fan_update_rate * 1000))

        self.populate_widgets()
        self.connect_signals()

    def populate_widgets(self):
        self.populate_combo_boxes_()
        self.populate_status_buttons_()
        self.populate_curves_()

    def populate_combo_boxes_(self):
        self.gui.cb_temperature_display_selection.clear()
        self.gui.cb_temperature_display_selection.addItems(
            [temp_config.name for temp_config in self.db.temperature_configs])
        self.gui.cb_fan_curve_selection.clear()
        self.gui.cb_fan_curve_selection.addItems(
            [fan_config.name for fan_config in self.db.fan_configs])
        self.gui.cb_led_curve_selection.clear()
        self.gui.cb_led_curve_selection.addItems(
            [led_config.name for led_config in self.db.led_configs])
        self.gui.cb_fan_curve_temperature_source_selection.clear()
        self.gui.cb_fan_curve_temperature_source_selection.addItems(
            [temp_config.name for temp_config in self.db.temperature_configs])
        self.gui.cb_led_curve_temperature_source_selection.clear()
        self.gui.cb_led_curve_temperature_source_selection.addItems(
            [temp_config.name for temp_config in self.db.temperature_configs])

        for idx, cb in enumerate([self.gui.cb_r_channel_sync, self.gui.cb_g_channel_sync, self.gui.cb_b_channel_sync]):
            cb.addItems(["SELF", "R Channel", "G Channel", "B Channel"])

            view = cb.view()  # type: QtWidgets.QListView
            view.setRowHidden(idx + 1, True)
            model = cb.model()  # type: QtGui.QStandardItemModel
            item = model.item(idx + 1)  # type: QtGui.QStandardItem
            item.setFlags(item.flags() & ~QtCore.Qt.ItemIsEnabled)

        self.gui.cb_fan_options_preset.addItems([fan_preset.name for fan_preset in self.db.fan_presets])
        self.gui.cb_led_options_preset.addItems([led_preset.name for led_preset in self.db.led_presets])

    def populate_status_buttons_(self):
        for idx, pb in enumerate(self.gui.rgb_preview_buttons):
            pb.setText(self.db.led_configs[idx].name)
            pb.setFixedHeight(pb.height() * 1.75)
        for idx, widget in enumerate(self.gui.fan_widgets):
            widget.name_button.setText(self.db.fan_configs[idx].name)

    def populate_curves_(self):
        self.on_cb_fan_curve_selection_currentIndexChanged(self.gui.cb_fan_curve_selection.currentIndex())
        self.on_cb_led_curve_selection_currentIndexChanged(self.gui.cb_led_curve_selection.currentIndex())

        self.gui.temperature_series.widget.getPlotItem().getViewBox().disableAutoRange()
        self.gui.temperature_series.widget.getPlotItem().getViewBox().setRange(
            xRange=(0, self.db.gui_config.max_temperature_sample_count),
            yRange=(self.db.gui_config.min_temperature_display, self.db.gui_config.max_temperature_display))

        # print(self.gui.temperature_series.graph.getViewWidget().setBackgroundBrush())

    def connect_signals(self):
        self.gui.cb_fan_curve_selection.currentIndexChanged.connect(self.on_cb_fan_curve_selection_currentIndexChanged)
        self.gui.cb_led_curve_selection.currentIndexChanged.connect(self.on_cb_led_curve_selection_currentIndexChanged)
        self.gui.fan_curve.graph.callback = self.on_fan_curve_dataChangeEvent
        for curve in self.gui.rgb_channel_curves:
            curve.graph.callback = lambda data_index: self.on_rgb_channel_curves_dataChangeEvent(data_index,
                                                                                                 syncing=False)
        self.gui.cb_pwm_controlled.stateChanged.connect(self.on_cb_pwm_controlled_stateChanged)

        self.gui.cb_fan_curve_temperature_source_selection.currentIndexChanged.connect(
            self.on_cb_fan_curve_temperature_source_selection_currentIndexChanged)
        self.gui.cb_led_curve_temperature_source_selection.currentIndexChanged.connect(
            self.on_cb_led_curve_temperature_source_selection_currentIndexChanged)
        self.gui.dsb_speed_multiplier.valueChanged.connect(self.on_dsb_speed_multiplier_valueChanged)
        self.gui.rb_time_controlled.toggled.connect(self.on_rb_time_controlled_toggled)

        self.gui.cb_r_channel_sync.currentIndexChanged.connect(
            lambda index: self.on_cb_rgb_channel_sync_currentIndexChanged(0, index))
        self.gui.cb_g_channel_sync.currentIndexChanged.connect(
            lambda index: self.on_cb_rgb_channel_sync_currentIndexChanged(1, index))
        self.gui.cb_b_channel_sync.currentIndexChanged.connect(
            lambda index: self.on_cb_rgb_channel_sync_currentIndexChanged(2, index))

        self.gui.pb_fan_options_save.clicked.connect(self.on_pb_fan_options_save_clicked)
        self.gui.cb_fan_options_preset.lineEdit().editingFinished.connect(self.on_pb_fan_options_save_clicked)
        self.gui.pb_led_options_save.clicked.connect(self.on_pb_led_options_save_clicked)
        self.gui.cb_led_options_preset.lineEdit().editingFinished.connect(self.on_pb_led_options_save_clicked)

        self.gui.pb_fan_options_load.clicked.connect(self.on_pb_fan_options_load_clicked)
        self.gui.pb_led_options_load.clicked.connect(self.on_pb_led_options_load_clicked)

        self.gui.pb_fan_options_delete.clicked.connect(self.on_pb_fan_options_delete_clicked)
        self.gui.pb_led_options_delete.clicked.connect(self.on_pb_led_options_delete_clicked)

    def _update_led_display(self):
        led_params = json.loads(driver_process.get_all_led_parameters())[StyledGuiController.LED_PARAMS_JSON_KEY]
        for idx in range(leviathans_breath.NUM_LEDS()):
            led_param = next(led_param for led_param in led_params if led_param["channel"] == idx)
            brightness_levels = np.round(
                np.array([led_param["r_brightness"], led_param["g_brightness"], led_param["b_brightness"]]) * 255 / 100)
            text_brightness_levels = np.round(
                (100 - np.array(
                    [led_param["r_brightness"], led_param["g_brightness"], led_param["b_brightness"]])) * 255 / 100)
            pb = self.gui.rgb_preview_buttons[idx]
            pb.setStyleSheet(
                "background-color: rgba({}, {}, {}, 1); color: rgba({}, {}, {}, 1)".format(*brightness_levels,
                                                                                           *text_brightness_levels))

    def _update_fan_status(self):
        fan_params = json.loads(driver_process.get_all_fan_parameters())[StyledGuiController.FAN_PARAMS_JSON_KEY]
        fan_rpms = json.loads(driver_process.get_all_fan_rpms())[StyledGuiController.RPM_JSON_KEY]

        for idx, rpm in enumerate(fan_rpms):
            rpm = 0 if rpm is None else rpm
            widget = self.gui.fan_widgets[idx]
            widget.rpm_label.setText("{} RPM".format(round(rpm)))
            if rpm > 0:
                widget.fan.set_duration(round(1000 / rpm * 750))
                widget.fan.start_animation()
            else:
                widget.fan.stop_animation()

        for fan_param in fan_params:
            widget = self.gui.fan_widgets[fan_param["channel"]]
            if fan_param["pwm_controlled"]:
                widget.v_p_label.setText("{:.2f}%".format(fan_param["pwm"]))
            else:
                widget.v_p_label.setText("{:.2f}V".format(fan_param["voltage"]))

    def _update_temperature_graph(self):
        temperatures = json.loads(driver_process.get_all_temperatures())[StyledGuiController.TEMPERATURES_JSON_KEY]
        temperature = temperatures[self.gui.cb_temperature_display_selection.currentIndex()]

        try:
            points = self.gui.temperature_series.graph.get_points()
            indices = points[:, 0]
            readings = points[:, 1]
            points = points.tolist()
        except KeyError:
            points = []
            indices = []
            readings = []

        if len(points) < constants.MAX_TIME_SERIES_POINTS:
            points.append((len(points), temperature))
            self.gui.temperature_series.graph.setData(pos=np.array(points), size=1, pxMode=True)
            # self.gui.temperature_series.graph.set_points(np.array(points))
        else:
            readings = readings[1:].tolist()
            readings.append(temperature)
            self.gui.temperature_series.graph.set_points(np.stack((indices, readings), axis=-1))

        if len(readings) > 0:
            min_reading, max_reading = round_to_ten(np.min(readings)), round_to_ten(
                np.max(readings) + self.db.gui_config.temperature_y_axis_buffer)
            view_range = self.gui.temperature_series.widget.getPlotItem().getViewBox().viewRange()
            # print(view_range)

            # if min_reading <

    @QtCore.pyqtSlot()
    def on_pb_led_options_delete_clicked(self):
        preset_name = str(self.gui.cb_led_options_preset.currentText()).strip()
        if not preset_name:
            return

        self.gui.cb_led_options_preset.removeItem(self.gui.cb_led_options_preset.currentIndex())

        self.db.delete_led_preset(preset_name)
        self.db.save(constants.DB_FILE_NAME)

    @QtCore.pyqtSlot()
    def on_pb_fan_options_delete_clicked(self):
        preset_name = str(self.gui.cb_fan_options_preset.currentText()).strip()
        if not preset_name:
            return

        self.gui.cb_fan_options_preset.removeItem(self.gui.cb_fan_options_preset.currentIndex())

        self.db.delete_fan_preset(preset_name)
        self.db.save(constants.DB_FILE_NAME)

    @QtCore.pyqtSlot()
    def on_pb_led_options_load_clicked(self):
        preset_name = str(self.gui.cb_led_options_preset.currentText()).strip()
        if not preset_name:
            return

        preset = self.db.get_led_preset(preset_name)

        for idx, cb in enumerate([self.gui.cb_r_channel_sync, self.gui.cb_g_channel_sync, self.gui.cb_b_channel_sync]):
            cb.blockSignals(True)
            sync_target = preset.channel_sync[idx]
            if sync_target != idx:
                cb.setCurrentIndex(sync_target + 1)
            cb.blockSignals(False)

        for idx, editable_curve in enumerate(self.gui.rgb_channel_curves):
            editable_curve.graph.set_points(np.array(preset.curves[idx]))

        self.on_rgb_channel_curves_dataChangeEvent(syncing=False)

    @QtCore.pyqtSlot()
    def on_pb_fan_options_load_clicked(self):
        preset_name = str(self.gui.cb_fan_options_preset.currentText()).strip()
        if not preset_name:
            return

        preset = self.db.get_fan_preset(preset_name)
        self.gui.fan_curve.graph.set_points(np.array(preset.curve))
        self.on_fan_curve_dataChangeEvent()

    @QtCore.pyqtSlot()
    def on_pb_led_options_save_clicked(self):
        preset_name = str(self.gui.cb_led_options_preset.currentText()).strip()
        if not preset_name:
            return

        preset = db_config.LEDCurvePreset()
        for idx, curve in enumerate(self.gui.rgb_channel_curves):
            preset.curves[idx] = curve.graph.get_points().tolist()
        for idx, sync in enumerate(
                [self.gui.cb_r_channel_sync, self.gui.cb_g_channel_sync, self.gui.cb_b_channel_sync]):
            preset.channel_sync[idx] = sync.currentIndex() - 1
        preset.name = preset_name

        self.db.add_led_preset(preset)
        self.db.save(constants.DB_FILE_NAME)

    @QtCore.pyqtSlot()
    def on_pb_fan_options_save_clicked(self):
        preset_name = str(self.gui.cb_fan_options_preset.currentText()).strip()
        if not preset_name:
            return

        points = self.gui.fan_curve.graph.get_points()

        preset = db_config.FanCurvePreset()
        preset.curve = points.tolist()
        preset.name = preset_name

        self.db.add_fan_preset(preset)
        self.db.save(constants.DB_FILE_NAME)

    def sync_led_curves(self):
        for idx, cb in enumerate([self.gui.cb_r_channel_sync, self.gui.cb_g_channel_sync, self.gui.cb_b_channel_sync]):
            sync_target_idx = cb.currentIndex() - 1
            if sync_target_idx < 0:
                continue

            us = self.gui.rgb_channel_curves[idx]
            them = self.gui.rgb_channel_curves[sync_target_idx]

            us.graph.set_points(them.graph.get_points())

        points = {
            StyledGuiController.RGB_CHAN_JSON_KEYS[idx]: self.gui.rgb_channel_curves[idx].graph.get_points().tolist()
            for idx in range(len(StyledGuiController.RGB_CHAN_JSON_KEYS))}

        curves_dict = {StyledGuiController.POINTS_JSON_KEY: points}
        driver_process.send_led_curve(self.gui.cb_led_curve_selection.currentIndex(), json.dumps(curves_dict))

    @QtCore.pyqtSlot(int, int)
    def on_cb_rgb_channel_sync_currentIndexChanged(self, cb_index, index):
        led_index = self.gui.cb_fan_curve_selection.currentIndex()
        self.db.led_configs[led_index].channel_sync[cb_index] = index - 1
        self.db.save(constants.DB_FILE_NAME)
        self.sync_led_curves()

    @QtCore.pyqtSlot(bool)
    def on_rb_time_controlled_toggled(self, checked):
        led_index = self.gui.cb_led_curve_selection.currentIndex()

        led_params = json.loads(driver_process.get_all_led_parameters())[StyledGuiController.LED_PARAMS_JSON_KEY]
        led_param = next(
            param for param in led_params if param["channel"] == led_index)

        if not checked:
            led_param["temperature_channel"] = self.gui.cb_led_curve_temperature_source_selection.currentIndex()

        led_param["time_controlled"] = checked
        driver_process.set_led_parameters(led_index, led_param["time_controlled"], led_param["speed_multiplier"],
                                          led_param["temperature_channel"])

    @QtCore.pyqtSlot(float)
    def on_dsb_speed_multiplier_valueChanged(self, value):
        self.spinner_timer.stop()
        self.spinner_timer.start(300)

    def set_speed_multiplier_(self):
        speed_multiplier = self.gui.dsb_speed_multiplier.value()

        led_index = self.gui.cb_led_curve_selection.currentIndex()

        led_params = json.loads(driver_process.get_all_led_parameters())[StyledGuiController.LED_PARAMS_JSON_KEY]
        led_param = next(
            param for param in led_params if param["channel"] == led_index)

        led_param["speed_multiplier"] = speed_multiplier
        driver_process.set_led_parameters(led_index, led_param["time_controlled"], led_param["speed_multiplier"],
                                          led_param["temperature_channel"])

    @QtCore.pyqtSlot(int)
    def on_cb_led_curve_temperature_source_selection_currentIndexChanged(self, index):
        if self.gui.rb_time_controlled.isChecked():
            return

        led_index = self.gui.cb_led_curve_selection.currentIndex()

        led_params = json.loads(driver_process.get_all_led_parameters())[StyledGuiController.LED_PARAMS_JSON_KEY]
        led_param = next(
            param for param in led_params if param["channel"] == led_index)

        led_param["temperature_channel"] = index
        driver_process.set_led_parameters(led_index, led_param["time_controlled"], led_param["speed_multiplier"],
                                          led_param["temperature_channel"])

    @QtCore.pyqtSlot(int)
    def on_cb_fan_curve_temperature_source_selection_currentIndexChanged(self, index):
        fan_index = self.gui.cb_fan_curve_selection.currentIndex()

        fan_params = json.loads(driver_process.get_all_fan_parameters())[StyledGuiController.FAN_PARAMS_JSON_KEY]
        fan_param = next(
            param for param in fan_params if param["channel"] == fan_index)

        fan_param["temperature_channel"] = index
        driver_process.set_fan_parameters(fan_index, fan_param["pwm_controlled"], fan_param["temperature_channel"])

    @QtCore.pyqtSlot(int)
    def on_cb_pwm_controlled_stateChanged(self, state):
        index = self.gui.cb_fan_curve_selection.currentIndex()
        fan_params = json.loads(driver_process.get_all_fan_parameters())[StyledGuiController.FAN_PARAMS_JSON_KEY]
        fan_param = next(param for param in fan_params if param["channel"] == index)

        fan_param["pwm_controlled"] = state == QtCore.Qt.Checked
        driver_process.set_fan_parameters(index, fan_param["pwm_controlled"], fan_param["temperature_channel"])

    @QtCore.pyqtSlot(int)
    def on_fan_curve_dataChangeEvent(self, data_index=-1):
        points = self.gui.fan_curve.graph.get_points()
        curve_dict = {StyledGuiController.POINTS_JSON_KEY: points.tolist()}
        # driver_process.send_fan_curve(self.gui.cb_fan_curve_selection.currentIndex(), json.dumps(curve_dict))

    @QtCore.pyqtSlot(int)
    def on_rgb_channel_curves_dataChangeEvent(self, data_index=-1, syncing=True):
        if syncing:
            return

        self.sync_led_curves()

    @QtCore.pyqtSlot(int)
    def on_cb_fan_curve_selection_currentIndexChanged(self, index):
        curve_json = driver_process.get_fan_curve(index)
        curve = json.loads(curve_json)[StyledGuiController.POINTS_JSON_KEY]
        n_curve = np.array(curve)
        self.gui.fan_curve.graph.setData(pos=n_curve, size=10, pxMode=True)

        fan_params = json.loads(driver_process.get_all_fan_parameters())[StyledGuiController.FAN_PARAMS_JSON_KEY]
        fan_param = next(param for param in fan_params if param["channel"] == index)

        self.gui.cb_pwm_controlled.setChecked(fan_param["pwm_controlled"])
        self.gui.cb_fan_curve_temperature_source_selection.setCurrentIndex(fan_param["temperature_channel"])

    @QtCore.pyqtSlot(int)
    def on_cb_led_curve_selection_currentIndexChanged(self, index):
        curves_json = driver_process.get_led_curve(index)
        curves = json.loads(curves_json)[StyledGuiController.POINTS_JSON_KEY]
        curves_list = [np.array(curves[chan]) for chan in StyledGuiController.RGB_CHAN_JSON_KEYS]
        for curve, gui_curve in zip(curves_list, self.gui.rgb_channel_curves):
            gui_curve.graph.setData(pos=curve, size=10, pxMode=True)

        led_params = json.loads(driver_process.get_all_led_parameters())[StyledGuiController.LED_PARAMS_JSON_KEY]
        led_param = next(param for param in led_params if param["channel"] == index)

        self.gui.dsb_speed_multiplier.setValue(led_param["speed_multiplier"])
        if led_param["time_controlled"]:
            self.gui.rb_time_controlled.setChecked(True)
            self.gui.rb_temperature_controlled.setChecked(False)
        else:
            self.gui.rb_time_controlled.setChecked(False)
            self.gui.rb_temperature_controlled.setChecked(True)
        self.gui.cb_led_curve_temperature_source_selection.setCurrentIndex(led_param["temperature_channel"])

        for idx, cb in enumerate([self.gui.cb_r_channel_sync, self.gui.cb_g_channel_sync, self.gui.cb_b_channel_sync]):
            cb.blockSignals(True)
            cb.setCurrentIndex(self.db.led_configs[index].channel_sync[idx] + 1)
            cb.blockSignals(False)


def round_to_ten(value):
    return int(value / 10) * 10
