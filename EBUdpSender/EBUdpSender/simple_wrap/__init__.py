# -*- coding:utf-8 -*-

import time
import sys
import threading
import EBUdpWrap as Eb
from typing import List


_pic_id_auto = {}


def create_udp_client_brick(group_id: int, host: str, port: int, client_num)->int:
    """
    :param group_id: 客户端组号, <65535
    :param host:接收端ip
    :param port:接收端端口
    :param client_num:申请客户端个数
    :return:
    """
    return Eb.CreateUdpSocketList(group_id, host, port, client_num)


def release_udo_client_brick(brick_index: int):
    """clse sockets"""
    return Eb.ReleaseSocket(brick_index)


def send_data_out(brick_index: int, data: bytes, width: int,  height: int, sample: int, u_sec=400)->int:
    """
    :param brick_index:
    :param data:
    :param width:  <65535
    :param height: <65535
    :param sample: <65535
    :param u_sec:
    :return: -1:failed, 0:success
    """
    _id = _pic_id_auto.get(brick_index, None)
    if _id is None:
        _id = 1
        _pic_id_auto[brick_index] = _id
    else:
        _pic_id_auto[brick_index] = _id+1 if _id < 65535 else 0
    return Eb.SendDataOut(brick_index, data, _id, width, height, sample, u_sec)


def start_listen_noblock(port: int):
    """return -1:failed, 0:success"""
    Eb.StartListener(port)


def stop_listen(port: int):
    Eb.StopListen(port)


def get_pic(group: int):
    """
    :param group: <65535
    :return: width, height, channel, data, data_len
    """
    return Eb.GetPic(group)


def get_groups()->List[int]:
    """return """
    return Eb.GetCurGroups()


def test_server(port=9009, mutable=[True]):
    start_listen_noblock(port)
    while mutable[0]:
        time.sleep(10)
        w, h, c, dt = get_pic(10)  # server so
        dt = dt if dt is None else sum(list(dt))
        print(w, h, c, dt)
        print("groups", get_groups())


def test_client(port=9009, mutable=[True]):
    import numpy as np
    index = create_udp_client_brick(10, "127.0.0.1", port, 5)
    print("index num", index)
    width0 = 1080
    height0 = 980
    sample0 = 3
    pic = np.ones(shape=(width0, height0, sample0), dtype=np.uint8)
    b_str = pic.tobytes()
    for _ in range(30):
        ret = send_data_out(index, b_str, width0, height0, sample0)
        print("send success" if ret==0 else "send failed")
        time.sleep(5)
    mutable[0] = False


def test_all(port=9009):
    from threading import Thread
    mutable = [True]
    t = Thread(target=test_server, args=(port, mutable))
    t.setDaemon(True)
    t.start()
    test_client(port, mutable)

