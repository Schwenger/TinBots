#!/usr/bin/env python3
# -*- coding:utf-8 -*-

from colorsys import rgb_to_hsv
from math import pi, sqrt
from PIL import Image


# ===== Configuration / magic numbers =====

bot_hues = [0.55, 0.60, 0.65, 0.85, 0.90, 0.95]  # on the range [0, 1]

max_hue_diff = 64  # on the range [0, 255]

visualize = True


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


def resolve(n, w):
    x = int(n % w)  # This int() is cosmetic
    y = int(n / w)  # This int() is necessary
    return (x, y)


def find_center(img):
    x_akku, y_akku, v_akku = 0.0, 0.0, 0.0
    w, _ = img.size
    max_v = 0
    for (n, v) in enumerate(img.getdata()):
        if v > 0:
            # FIXME: I'm sure there's a better way.
            (x, y) = resolve(n, w)
            x_akku = x_akku + x * v
            y_akku = y_akku + y * v
            v_akku = v_akku + v
            max_v = max(max_v, v)
    if v_akku < 1:
        return (0, 0, 0)
    theoretical_radius = sqrt((v_akku / max_v) / pi)
    return (x_akku / v_akku, y_akku / v_akku, theoretical_radius)


def overlay_center(img_orig, hue, center):
    from colorsys import hsv_to_rgb
    from PIL import ImageDraw

    img = img_orig.copy()
    anti_hue = (hue + 0.5) % 1
    rgb = tuple([int(255 * c) for c in hsv_to_rgb(anti_hue, 1, 1)])
    draw = ImageDraw.Draw(img)
    x, y, r = center
    draw.ellipse([x - r, y - r, x + r, y + r], fill=None, outline=rgb)
    return img


# ===== Detector class =====

class BlobDetector:
    # ideal members: dst_hue, cur_hue, raw_result, dir_compensated_result
    # 'result' is tuple of x, y, phi
    # Note that x and y are on the image, so axes are "→x" and "↓y"

    # actual members: hue, raw_scores, raw_center
    # 'visualize' members: raw_center_overlay

    def __init__(self, hue):
        self.hue = hue

    def analyze(self, img_in):
        # First, find out which pixels are relevant
        self.raw_scores = pixelwise(img_in, score_lambda(self.hue))
        self.raw_center = find_center(self.raw_scores)
        if visualize:
            self.raw_center_overlay = overlay_center(img_in, hue, self.raw_center)


# ===== Example call =====

if __name__ == '__main__':
    from sys import argv

    if len(argv) > 2:
        print("Usage: ./blob_detection.py [path/to/input.png]")
        exit(1)
    if len(argv) == 2:
        img = Image.open(argv[1])
    else:
        img = Image.open('../../../es_heavy/cbp_tests/gen_320x240/PICT0001.JPG.png')
    for hue in bot_hues:
        print("Running with hue {} ...".format(hue))
        bd = BlobDetector(hue)
        bd.analyze(img)
        if visualize:
            with open('analysis_{}_raw_center.png'.format(hue), 'wb') as out_file:
                bd.raw_center_overlay.save(out_file, 'png')
    print("Done!")
