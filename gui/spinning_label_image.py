import os
import constants

from PyQt5 import QtCore, QtGui, QtWidgets, Qt


class SpinningLabelImage(QtWidgets.QLabel):
    def __init__(self, *args, **kwargs):
        super(SpinningLabelImage, self).__init__(*args, **kwargs)
        self._pixmap = QtGui.QPixmap()
        self._animation = QtCore.QVariantAnimation(
            self,
            startValue=0.0,
            endValue=360.0,
            duration=10,
            valueChanged=self.on_valueChanged,
            loopCount=-1
        )
        self.transform = QtGui.QTransform()

    def set_pixmap(self, pixmap):
        self._pixmap = pixmap
        self.setPixmap(self._pixmap)

    def start_animation(self):
        if self._animation.state() != QtCore.QAbstractAnimation.Running:
            self._animation.start()

    def stop_animation(self):
        if self._animation.state() == QtCore.QAbstractAnimation.Running:
            self._animation.stop()

    def set_duration(self, duration):
        self._animation.setDuration(duration)

    @QtCore.pyqtSlot(QtCore.QVariant)
    def on_valueChanged(self, value):
        # t = QtGui.QTransform()
        # t.rotate(value)
        self.transform.rotate(value)
        self.setPixmap(self._pixmap.transformed(self.transform))


class Widget(QtWidgets.QWidget):
    def build_label(self):
        label = SpinningLabelImage(alignment=QtCore.Qt.AlignCenter)
        img_path = os.path.join(constants.APP_ROOT_DIR, "fan.svg")
        pixmap = QtGui.QPixmap(img_path)
        pixmap = pixmap.scaledToHeight(80, QtCore.Qt.SmoothTransformation)
        label.set_pixmap(pixmap)

        return label

    def __init__(self, parent=None):
        super(Widget, self).__init__(parent)
        button = QtWidgets.QPushButton('Rotate')
        lay = QtWidgets.QVBoxLayout(self)
        for idx in range(8):
            label = self.build_label()

            button.clicked.connect(label.start_animation)

            lay.addWidget(label)
        lay.addWidget(button)


if __name__ == '__main__':
    import sys

    app = QtWidgets.QApplication(sys.argv)
    w = Widget()
    w.show()

    sys.exit(app.exec_())
