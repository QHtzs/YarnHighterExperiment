# -*- coding:utf-8 -*-
"""
性能瓶颈在MTCNN，故QUEUE作为进程间通讯介质可行
"""
from PIL import Image
from facenet.src.align import detect_face
from facenet.src import facenet
import numpy as np
import tensorflow as tf
import os
from typing import Dict
from multiprocessing.dummy import Process, Queue

__all__ = ["Recon"]


class MtCNN(Process):
    def __init__(self, process_queue_in: Queue, process_queue_out: Queue):
        super().__init__()
        self.inq = process_queue_in
        self.out_q = process_queue_out

    def run(self):
        minsize = 20  # minimum size of face
        threshold = [0.6, 0.7, 0.7]  # three steps's threshold
        factor = 0.709  # scale factor
        margin = 44
        image_size = 160
        gpu_memory_fraction = 1.0
        with tf.Graph().as_default():
            gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=gpu_memory_fraction)
            sess = tf.Session(config=tf.ConfigProto(gpu_options=gpu_options, log_device_placement=False))
            with sess.as_default():
                p_net, r_net, o_net = detect_face.create_mtcnn(sess, None)
        while True:
            img, group = self.inq.get()
            img_size = np.asarray(img.shape)[0:2]
            bounding_boxes, _ = detect_face.detect_face(img, minsize, p_net, r_net, o_net, threshold, factor)
            src = img.copy()
            dist_white_ends = []
            for num in range(bounding_boxes.shape[0]):
                det = np.squeeze(bounding_boxes[num, 0:4])
                bb = np.zeros(4, dtype=np.int32)
                bb[0] = np.maximum(det[0] - margin / 2, 0)
                bb[1] = np.maximum(det[1] - margin / 2, 0)
                bb[2] = np.minimum(det[2] + margin / 2, img_size[1])
                bb[3] = np.minimum(det[3] + margin / 2, img_size[0])
                cropped = img[bb[1]:bb[3], bb[0]:bb[2], :]

                if (bb[0] >= 0) & (bb[0] < src.shape[1]):
                    src[bb[1]:bb[3], bb[0], :] = 255
                else:
                    src[bb[1]:bb[3], src.shape[1]-1, :] = 255

                if (bb[2] >= 0) & (bb[2] < src.shape[1]):
                    src[bb[1]:bb[3], bb[2], :] = 255
                else:
                    src[bb[1]:bb[3], src.shape[1]-1, :] = 255

                if (bb[1] >= 0) & (bb[1] < src.shape[0]):
                    src[bb[1], bb[0]:bb[2], :] = 255
                else:
                    src[src.shape[0]-1, bb[0]:bb[2], :] = 255

                if (bb[3] >= 0) & (bb[3] < src.shape[0]):
                    src[bb[3], bb[0]:bb[2], :] = 255
                else:
                    src[src.shape[0] - 1, bb[0]:bb[2], :] = 255

                if (cropped.shape[0] < 10) or (cropped.shape[1] < 10):
                    continue   # 形状过小(可能非人脸)， 或者距离过远
                pil_im = Image.fromarray(cropped)
                aligned = pil_im.resize((image_size, image_size), Image.BILINEAR)
                aligned = np.array(aligned)
                pre_whitened = facenet.prewhiten(aligned)
                dist_white_ends.append(pre_whitened)
            self.out_q.put({"src": src, "dst": dist_white_ends, "group":group})


class EmbeddingCmp(Process):
    def __init__(self, mt_i_queue: Queue, mt_o_queue: Queue, process_queue_out: Queue,
                 path_group_dict: Dict[str, int], model_path: str):
        super().__init__()
        self.fd_q = mt_i_queue
        self.i_q = mt_o_queue
        self.o_q = process_queue_out
        self.group_name_pics = {}
        self.model_path = model_path
        for p_path, group in path_group_dict.items():
            files = os.listdir(p_path)
            abs_paths = [os.path.join(p_path, x) for x in files]
            for i in range(abs_paths.__len__()):
                name = abs_paths[i]
                image = Image.open(name)
                image = image.convert("RGB")
                image = np.array(image)
                self.fd_q.put((image, group))
                dist_white_ends_dict = self.i_q.get()
                dist_white_ends = dist_white_ends_dict["dst"]
                if dist_white_ends:
                    if self.group_name_pics.get(dist_white_ends_dict["group"], None) is None:
                        self.group_name_pics[dist_white_ends_dict["group"]] = {name: dist_white_ends[0][None]}
                    else:
                        self.group_name_pics[dist_white_ends_dict["group"]].update({name: dist_white_ends[0][None]})

    def run(self):
        with tf.Graph().as_default():
            with tf.Session() as sess:
                facenet.load_model(self.model_path)
                images_placeholder = tf.get_default_graph().get_tensor_by_name("input:0")
                embeddings = tf.get_default_graph().get_tensor_by_name("embeddings:0")
                phase_train_placeholder = tf.get_default_graph().get_tensor_by_name("phase_train:0")
                all_keys = self.group_name_pics.keys()
                for group in all_keys:
                    name_pic_dict = self.group_name_pics[group]
                    for name in name_pic_dict.keys():
                        feed_dict = {images_placeholder: name_pic_dict[name], phase_train_placeholder: False}
                        emb0 = sess.run(embeddings, feed_dict=feed_dict)
                        self.group_name_pics[group][name] = emb0

                while True:
                    image_dict = self.i_q.get()
                    dist_white_ends = image_dict["dst"]
                    group = image_dict["group"]
                    label_distance = []
                    for white_end in dist_white_ends:
                        label = ""
                        f_min = np.Inf
                        image = white_end[None]
                        feed_dict = {images_placeholder: image, phase_train_placeholder: False}
                        emb = sess.run(embeddings, feed_dict=feed_dict)
                        name_pic_dict = self.group_name_pics[group]
                        for k, v in name_pic_dict.items():
                            distance = np.sqrt(np.sum(np.square(np.subtract(emb[0:], v[0:]))))
                            if distance < f_min:
                                f_min = distance
                                label = k
                        label_distance.append((label, f_min))
                    image_dict["ret"] = label_distance
                    self.o_q.put(image_dict)
                    # {"src": src, "dst": dist_white_ends, "group":group, "ret":(label, f_min)}


class Recon:
    def __init__(self, model_dir, path_group_dict: Dict[str, int]):
        self.feed = Queue()
        self.mid = Queue()
        self.out = Queue()
        t = MtCNN(self.feed, self.mid)
        t.start()
        t1 = EmbeddingCmp(self.feed, self.mid, self.out, path_group_dict, model_dir)
        t1.start()

    def face_check(self, _im: np.ndarray, group : int):
        self.feed.put((_im, group) )
        return self.out.get()
