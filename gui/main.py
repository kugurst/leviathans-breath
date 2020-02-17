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
