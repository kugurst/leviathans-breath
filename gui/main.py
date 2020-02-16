import sys
import pyqtgraph as pg
import qdarkstyle

import styled_gui
import leviathans_breath

from PyQt5 import QtCore, QtGui, QtWidgets


def setup_gui():
    pg.setConfigOptions(antialias=True)

    app = QtWidgets.QApplication(sys.argv)

    with open("gui.css", "r") as css_file:
        css = css_file.read()
    app.setStyleSheet(qdarkstyle.load_stylesheet(qt_api='pyqt5') + "\n" + css)

    mw_main = QtWidgets.QMainWindow()
    ui = styled_gui.StyledGui()
    ui.setupUi(mw_main)

    return ui, mw_main, app


def setup_driver():
    driver = leviathans_breath.Driver()
    return driver


def main():
    driver = setup_driver()
    print(driver.connect())
    print(driver.get_all_temperatures())
    print(driver.get_fan_curve(1))
    ui, mw_main, app = setup_gui()

    mw_main.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
