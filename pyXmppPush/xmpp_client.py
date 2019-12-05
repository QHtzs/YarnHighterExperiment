# -*- coding:utf-8 -*-

from PyQt5.QtCore import QObject


class PyQtClientWrap(QObject):
    """ q application is need"""
    def __init__(self, parent=None):
        import EbXmpp
        super().__init__(parent)
        self.global_client = EbXmpp

    def set_no_verify(self):
        self.global_client.pSetIgnoreSslError()

    def add_ca_certificate(self, base64_pem: bytes):
        if isinstance(base64_pem, str):
            base64_pem = base64_pem.encode("UTF-8")
        if isinstance(base64_pem, bytes):
            self.global_client.pAddCaCertificate(base64_pem)

    def connect_to_host(self, jid: bytes, pwd: bytes):
        if isinstance(jid, str):
            jid = jid.encode("UTF-8")
        if isinstance(pwd, str):
            pwd = pwd.encode("UTF-8")
        if isinstance(jid, bytes) and isinstance(pwd, bytes):
            self.global_client.pConnectToServer(jid, pwd)

    def send_message(self, jid: bytes, msg: bytes):
        return self.global_client.pSendMessage(jid, msg)

    def send_file(self, jid: bytes, file_path: bytes):
        return self.global_client.pSendImage(jid, file_path)


if __name__ == '__main__':
    from PyQt5.QtCore import QCoreApplication
    import time
    app = QCoreApplication([])
    xmpp = PyQtClientWrap()
    xmpp.set_no_verify()
    xmpp.add_ca_certificate(b"XDF=")
    print("a")
    xmpp.connect_to_host(b"test@xx.111", b"test")
    print("b")
    time.sleep(3)
    xmpp.send_message(b"test1@xx.111", b"ooo")
    print("c")
    xmpp.send_file(b"test1@xx.111", br"C:\Users\user\Desktop\pyd\Makefile")
    print("d")
    app.exec()
