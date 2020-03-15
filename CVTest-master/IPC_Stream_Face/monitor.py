# -*- coding:utf-8 -*-

import cv2
import numpy as np
from simple_wrap import start_listen_noblock, get_pic   # 自制python extension 见 UtilModule分支EBUdpSender
from PyQt5.QtWidgets import QMainWindow, QWidget
from PyQt5.Qt import QImage,  QPalette, QBrush, QApplication
# from xmpp_client import PyQtClientWrap # 自制python extension,见 UtilModule分支pyXmppPush
from facenet_wraper import Recon
from threading import Thread
from config import *
import platform


class NeedShow(QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("测试用")
        self.widget = QWidget(self)
        self.widget.setAutoFillBackground(True)
        self.setCentralWidget(self.widget)

    def update_image(self, array, label):
        im = cv2.transpose(array)
        im = im.transpose((1, 0, 2)).copy()
        image = QImage(im, im.shape[1], im.shape[0], QImage.Format_RGB888)
        palette = self.widget.palette()
        image = image.scaled(self.widget.geometry().width(), self.widget.geometry().height())
        palette.setBrush(QPalette.Background, QBrush(image))
        self.widget.setPalette(palette)
        self.widget.update()
        self.setWindowTitle(label)


class Monitor(Thread):
    def __init__(self, groups, recn: Recon, noti = None, win = None):
        super().__init__()
        self.groups = groups
        self.noti = noti
        self.render = win
        self.recn = recn

    def load_picture(self):
        for group in self.groups:
            w, h, c, dt = get_pic(group)
            yield w, h, c, dt, group

    def run(self):
        i = 0
        while True:
            for w, h, c, dt, group in self.load_picture():
                if isinstance(dt, bytes) and w > 0:
                    array = np.frombuffer(dt, dtype=np.uint8).reshape((h, w, c))
                    array = array.transpose((1, 0, 2)).copy()
                    array = cv2.transpose(array)
                    ret = self.recn.face_check(array, group)
                    # ret type is {"src": src, "dst": dist_white_ends, "group":group, "ret":[(label, f_min),..]}
                    bl = 0
                    if self.render is not None:
                        label_distance = ret["ret"]
                        label = ""
                        for l_s in label_distance:
                            if l_s[1] > 1.2:  # 超出阈值，不属于收录的任何人，为陌生人
                                bl = bl if bl else 1
                            else:
                                bl = 2
                            label += "%s:%s " % (l_s[0], l_s[1])
                        self.render.update_image(ret["src"], label)
                    if bl == 1 and (self.noti is not  None):
                        name = "im%i_%i.png" % (group, i)
                        i += 1
                        im = ret["src"]
                        if ftp_dirs_map:
                            name = os.path.join(ftp_dirs_map.get(group, ""), name)
                            cv2.imwrite(name, im)
                            os.system("chmod 500 %s" % name)
                        elif platform.platform().lower().__contains__("window"):
                            pass
                        else:
                            name = os.path.join(tmp_dir, name)
                            cv2.imwrite(name, im)
                        self.noti.send_message(udpgroup_and_xmmpusername[group], b"stranger in picture")
                        # 不支持离线文件，不大友好。
                        # self.noti.send_file(udpgroup_and_xmmpusername[group]+b"/QXmpp", name.encode())


def start_monitor(groups, port, need_win=False):
    recn = Recon(model_dir_, path_imagegroup_map)
    start_listen_noblock(port)
    app = QApplication([])
    # xmpp = PyQtClientWrap(None)
    # xmpp.set_no_verify()
    # xmpp.connect_to_host(b"test1@%s" % xmpp_server_host, b"test")
    xmpp = None
    win = None
    if need_win:
        win = NeedShow(None)
        win.show()
    t = Monitor(groups, recn, xmpp, win)
    t.setDaemon(True)
    t.start()
    app.exec()
    # t.run()
    

if __name__ == '__main__':
    start_monitor([10, 11], 9009, True)