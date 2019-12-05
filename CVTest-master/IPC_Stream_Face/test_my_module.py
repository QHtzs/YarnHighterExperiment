# -*- coding:utf-8 -*-

from simple_wrap import start_listen_noblock, get_pic
import cv2
import numpy as np
from PyQt5.QtWidgets import QMainWindow, QWidget
from PyQt5.Qt import QImage, QTimer, QPalette, QBrush, QApplication


class MyWindow(QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("测试用")
        self.widget = QWidget(self)
        self.widget.setAutoFillBackground(True)
        self.setCentralWidget(self.widget)
        start_listen_noblock(9009)
        self.timer = QTimer()
        self.timer.setSingleShot(False)
        self.timer.timeout.connect(self.update_image)
        self.timer.start(300)

    def update_image(self):
        w, h, c, dt = get_pic(10)
        if not isinstance(dt, bytes):
            return
        # array = np.frombuffer(dt, dtype=np.uint8).reshape((h, w, c))
        # array = array.transpose((1, 0, 2)).copy()
        # array = cv2.transpose(array)
        # cv2.imshow("aa", array)
        image = QImage(dt, w, h, QImage.Format_RGB888)
        palette = self.widget.palette()
        image = image.scaled(self.widget.geometry().width(), self.widget.geometry().height())
        palette.setBrush(QPalette.Background, QBrush(image))
        self.widget.setPalette(palette)
        self.widget.update()


if __name__ == '__main__':
    src = [
        "rtsp://admin:HikRASYVE@192.168.0.202/Streaming/Channels/1",
        "rtmp://media3.sinovision.net:1935/live/livestream",
        "http://ivi.bupt.edu.cn/hls/cctv6hd.m3u8",
    ]
    q = QApplication([])
    w = MyWindow(None)
    w.show()
    q.exec()