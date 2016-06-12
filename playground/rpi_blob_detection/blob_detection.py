#!/usr/bin/env python3
# -*- coding:utf-8 -*-

from colorsys import rgb_to_hsv
from PIL import Image


# ===== default stuff.  Delete me! =====

example = Image.open('../../../es_heavy/cbp_tests/gen_300x200/PICT0001.JPG.png')

initial_hues = [0.90, 0.60]

max_hue_diff = 64


# ===== Basic building blocks =====

def pixelwise(img_in, fn):
    img_out = Image.new("L", img_in.size)
    img_out.putdata([fn(px) for px in img_in.getdata()])
    return img_out


def score(dst_hue, rgb):
    h, s, v = rgb_to_hsv(*[x / 255.0 for x in rgb])
    diff = abs(dst_hue - h) % 1.0
    diff = min(diff, 1 - diff)
    diff = int(diff * 256 * 2)
    # These threshold don't need any modification:
    if s < 0.3 or v < 0.1:
        diff = 255
    if diff > max_hue_diff:
        diff = 255
    return 255 - diff


def score_lambda(dst_hue):
    return lambda rgb: score(dst_hue, rgb)


# ===== Detector class =====

class BlobDetector:
    # ideal members: dst_hue, cur_hue, raw_result, dir_compensated_result
    # 'result' is tuple of x, y, phi
    # Note that x and y are on the image, so axes are "→x" and "↓y"

    # actual members: hue, scores
    # debug members: img_hue_diff, img_hue_chosen

    def __init__(self, hue):
        self.hue = hue

    def analyze(self, img_in):
        self.scores = pixelwise(img_in, score_lambda(self.hue))


# ===== Example call =====

if __name__ == '__main__':
    print('Hello world')
