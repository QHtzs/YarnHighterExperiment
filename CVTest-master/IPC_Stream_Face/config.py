# -*- coding:utf-8 -*-

import os
import tempfile

xmpp_server_host = "127.0.0.1".encode()
root_dir = os.path.dirname(os.path.realpath(__file__))
model_dir_ = os.path.join(root_dir, "open_source_models").replace("\\", "/")
for_test_dir_ = os.path.join(root_dir, "image_demo/group_10_11").replace("\\", "/")
udpgroup_and_imggroup_map = {10: 10, 11: 10}  # udp接收图片编号：用于模型比对图片组编号
path_imagegroup_map = {for_test_dir_: 10}  # 图片组所在路径:图片组编号
udpgroup_and_xmmpusername = {10:b"test@%s" % xmpp_server_host, 11:b"test@%s" % xmpp_server_host}  # 图片接收组，及消息接受者的xmpp jid
tmp_dir = os.path.split(tempfile.TemporaryDirectory().name)[0]

