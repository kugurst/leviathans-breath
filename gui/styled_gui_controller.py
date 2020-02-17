import enum
import os

import db_config
import driver_process
import json
import numpy as np
import styled_gui
import yaml

from PyQt5 import QtCore, QtGui, QtWidgets

DB_FILE_NAME = os.path.join(os.path.dirname(os.path.abspath(__file__)), "db.yaml")
MAX_TEMPERATURE_POINTS = 60


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
    LED_PARAMS_JSON_KEY = "leds"

    def __init__(self, gui: styled_gui.StyledGui):
        super(self.__class__, self).__init__()
        self.gui = gui
        self.db = db_config.DB.build_default()

        self.spinner_timer = QtCore.QTimer(self)
        self.spinner_timer.timeout.connect(self.set_speed_multiplier_)
        self.spinner_timer.setSingleShot(True)

        if os.path.isfile(DB_FILE_NAME):
            try:
                self.db.load(DB_FILE_NAME)
            except yaml.YAMLError:
                os.unlink(DB_FILE_NAME)

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

    def populate_status_buttons_(self):
        for idx, pb in enumerate(self.gui.rgb_preview_buttons):
            pb.setText(self.db.led_configs[idx].name)
        for idx, widget in enumerate(self.gui.fan_widgets):
            widget.name_button.setText(self.db.fan_configs[idx].name)

    def populate_curves_(self):
        self.on_cb_fan_curve_selection_currentIndexChanged(self.gui.cb_fan_curve_selection.currentIndex())
        self.on_cb_led_curve_selection_currentIndexChanged(self.gui.cb_led_curve_selection.currentIndex())

    def connect_signals(self):
        self.gui.cb_fan_curve_selection.currentIndexChanged.connect(self.on_cb_fan_curve_selection_currentIndexChanged)
        self.gui.cb_led_curve_selection.currentIndexChanged.connect(self.on_cb_led_curve_selection_currentIndexChanged)
        self.gui.fan_curve.graph.callback = self.on_fan_curve_dataChangeEvent
        for curve in self.gui.rgb_channel_curves:
            curve.graph.callback = lambda data_index: self.on_rgb_channel_curves_dataChangeEvent(data_index, syncing=False)
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

    def sync_led_curves(self):
        for idx, cb in enumerate([self.gui.cb_r_channel_sync, self.gui.cb_g_channel_sync, self.gui.cb_b_channel_sync]):
            sync_target_idx = cb.currentIndex() - 1
            if sync_target_idx < 0:
                continue

            us = self.gui.rgb_channel_curves[idx]
            them = self.gui.rgb_channel_curves[sync_target_idx]

            us.graph.set_points(them.graph.get_points())

        points = {StyledGuiController.RGB_CHAN_JSON_KEYS[idx]: quarter_round(
            self.gui.rgb_channel_curves[idx].graph.get_points()).tolist() for
                  idx in range(len(StyledGuiController.RGB_CHAN_JSON_KEYS))}

        curves_dict = {StyledGuiController.POINTS_JSON_KEY: points}
        driver_process.send_led_curve(self.gui.cb_led_curve_selection.currentIndex(), json.dumps(curves_dict))

    @QtCore.pyqtSlot(int, int)
    def on_cb_rgb_channel_sync_currentIndexChanged(self, cb_index, index):
        led_index = self.gui.cb_fan_curve_selection.currentIndex()
        self.db.led_configs[led_index].channel_sync[cb_index] = index - 1
        self.db.save(DB_FILE_NAME)
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
    def on_fan_curve_dataChangeEvent(self, data_index):
        points = self.gui.fan_curve.graph.get_points()
        points = quarter_round(points)
        curve_dict = {StyledGuiController.POINTS_JSON_KEY: points.tolist()}
        driver_process.send_fan_curve(self.gui.cb_fan_curve_selection.currentIndex(), json.dumps(curve_dict))

    @QtCore.pyqtSlot(int)
    def on_rgb_channel_curves_dataChangeEvent(self, data_index, syncing=True):
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


def quarter_round(x):
    return np.round(x * 4) / 4
