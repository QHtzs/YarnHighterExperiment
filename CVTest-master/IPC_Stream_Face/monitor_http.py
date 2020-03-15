# -*- coding:utf-8 -*-


"""
对采样要求不高 最高接收处理2帧/秒 即可
不做私有协议，采用现成的http库做简单处理
"""

import cv2
import numpy as np
from facenet_wraper import Recon
from flask import Flask, request, jsonify
from config import *
from threading import Lock
import time


__recon_global = Recon(model_dir_, path_imagegroup_map)
__thread_lock = Lock()


def parse_data(data: bytes)->(np.ndarray, int):
    if len(data) < 8:
        return np.zeros([0], dtype=np.uint8), -1
    w = data[0] * 256 + data[1]
    h = data[2] * 256 + data[3]
    c = data[4] * 256 + data[5]
    group = data[6] * 256 + data[7]
    check_size = w * h * c
    real_size = len(data) - 8
    if check_size != real_size:
        return np.zeros([0], dtype=np.uint8), -1
    array = np.frombuffer(data[8:], dtype=np.uint8).reshape((h, w, c))
    array = array.transpose((1, 0, 2)).copy()
    array = cv2.transpose(array)
    return array, group


def image_check(data: bytes):
    array, group = parse_data(data)
    # if array.shape[0] > 1:
    #     cv2.imshow("im", array)
    #     cv2.waitKey()
    label = ""
    if group is -1:
        return label
    with __thread_lock:
        ret = __recon_global.face_check(array, group)
        # ret type is {"src": src, "dst": dist_white_ends, "group":group, "ret":[(label, f_min),..]}
    label_distance = ret["ret"]
    for l_s in label_distance:
        if l_s[1] > 1.2:  # 超出阈值，不属于收录的任何人，为陌生人
            name = "im%i_%i.png" % (group, time.time()*1000)
            label += "%s:%s " % (l_s[0], l_s[1])
            im = ret["src"]
            if ftp_dirs_map:
                name = os.path.join(ftp_dirs_map.get(group, ""), name)
                cv2.imwrite(name, im)
                os.system("chmod 500 %s" % name)
    return label


app = Flask("server")
# view


@app.route("/PIC", methods=["POST"])
def stream_recon():
    data = request.get_data()
    text = image_check(data)
    return jsonify({"label": text})


