import sys
import pyqtgraph as pg
import qdarkstyle

import driver_process
import leviathans_breath
import styled_gui
import styled_gui_controller

from PyQt5 import QtCore, QtGui, QtWidgets


def setup_gui():
    pg.setConfigOptions(antialias=True)

    app = QtWidgets.QApplication(sys.argv)

    with open("gui.css", "r") as css_file:
        css = css_file.read()
    app.setStyleSheet(qdarkstyle.load_stylesheet(qt_api='pyqt5') + "\n" + css)

    ui = styled_gui.StyledGui()
    ui.setupUi()

    return ui, app


def main():
    driver_process.start_driver_process()

    print(driver_process.connect())
    print(driver_process.is_connected())
    print(driver_process.sync())
    print(driver_process.get_fan_curve(1))
    print(driver_process.get_led_curve(1))
    print(driver_process.get_all_fan_rpms())
    print(driver_process.get_all_temperatures())
    print(driver_process.get_all_fan_parameters())
    print(driver_process.get_all_led_parameters())

    print(leviathans_breath.NUM_FANS())
    print(leviathans_breath.NUM_LEDS())
    print(leviathans_breath.NUM_LED_CHANNELS())
    print(leviathans_breath.NUM_TEMPERATURE_SENSORS())
    print(leviathans_breath.POINTS_PER_CURVE())

    ui, app = setup_gui()
    styled_gui_controller.StyledGuiController(ui)
    ui.show()

    ret = app.exec_()

    sys.exit(ret)


if __name__ == "__main__":
    try:
        main()
    finally:
        driver_process.stop_driver_process()
