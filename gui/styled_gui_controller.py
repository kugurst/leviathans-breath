import os

import db_config
import driver_process
import json
import numpy as np
import styled_gui
import yaml

from PyQt5 import QtCore, QtGui, QtWidgets


DB_FILE_NAME = os.path.join(os.path.dirname(os.path.abspath(__file__)), "db.yaml")


class StyledGuiController(QtWidgets.QWidget):
    POINTS_JSON_KEY = "points"
    RGB_CHAN_JSON_KEYS = ["r", "g", "b"]

    def __init__(self, gui: styled_gui.StyledGui):
        super(self.__class__, self).__init__()
        self.gui = gui
        self.db = db_config.DB.build_default()

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
        self.gui.fan_curve.graph.callback = self.on_fan_curve_mouseDragEvent

    @QtCore.pyqtSlot(int)
    def on_fan_curve_mouseDragEvent(self, data_index):
        print(data_index)

    @QtCore.pyqtSlot(int)
    def on_cb_fan_curve_selection_currentIndexChanged(self, index):
        curve_json = driver_process.get_fan_curve(index)
        curve = json.loads(curve_json)[StyledGuiController.POINTS_JSON_KEY]
        n_curve = np.array(curve)
        self.gui.fan_curve.graph.setData(pos=n_curve, size=10, pxMode=True)

    @QtCore.pyqtSlot(int)
    def on_cb_led_curve_selection_currentIndexChanged(self, index):
        curves_json = driver_process.get_led_curve(index)
        curves = json.loads(curves_json)[StyledGuiController.POINTS_JSON_KEY]
        curves_list = [np.array(curves[chan]) for chan in StyledGuiController.RGB_CHAN_JSON_KEYS]
        for curve, gui_curve in zip(curves_list, self.gui.rgb_channel_curves):
            gui_curve.graph.setData(pos=curve, size=10, pxMode=True)
