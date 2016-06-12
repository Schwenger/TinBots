#!/usr/bin/env python3
# -*- coding:utf-8 -*-

from colorsys import rgb_to_hsv
from math import atan2, pi, sqrt
from PIL import Image


# ===== Configuration / magic numbers =====

bot_hues = [0.55, 0.60, 0.65, 0.85, 0.90, 0.95]  # on the range [0, 1]

hue_tolerance_raw = 64  # on the range [0, 255]

hue_tolerance_cc = 20  # on the range [0, 255]

visualize = True


# ===== Basic building blocks =====

def pixelwise(img_in, fn):
    img_out = Image.new("L", img_in.size)
    img_out.putdata([fn(px) for px in img_in.getdata()])
    return img_out


def score(dst_hue, rgb, tolerance):
    h, s, v = rgb_to_hsv(*[x / 255.0 for x in rgb])
    diff = abs(dst_hue - h) % 1.0
    diff = min(diff, 1 - diff)
    diff = int(diff * 256 * 2)
    # These threshold don't need any modification:
    if s < 0.3 or v < 0.1:
        diff = 255
    if diff > tolerance:
        diff = 255
    return 255 - diff


def score_lambda(dst_hue, tolerance):
    return lambda rgb: score(dst_hue, rgb, tolerance)


def whiteish(rgb):
    _, s, v = rgb_to_hsv(*[x / 255.0 for x in rgb])
    # These threshold don't need any modification:
    return 255 if s < 0.3 and v > 0.8 else 0


def resolve(n, w):
    x = int(n % w)  # This int() is cosmetic
    y = int(n / w)  # This int() is necessary
    return (x, y)


def around(center, radius):
    return range(int(center - radius), int(center + radius) + 1)


# ===== High-level-ish computer-vision ... ish =====

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
        return None
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


def find_dot(whiteish, center):
    center_x, center_y, r = center
    x_akku, y_akku, v_akku = 0.0, 0.0, 0
    r = r * 0.8
    if r < 3:
        return None
    w, h = whiteish.size
    for py in around(center_y, r):
        for px in around(center_x, r):
            if px < 0 or px >= w or py < 0 or py >= h:
                # FIXME: Ugh
                continue
            x, y = px - center_x, py - center_y
            if x * x + y * y > r * r:
                continue
            if whiteish.getpixel((px, py)) == 0:
                continue
            x_akku = x_akku + x
            y_akku = y_akku + y
            v_akku = v_akku + 1
    if v_akku < 1:
        return None
    return (x_akku / v_akku, y_akku / v_akku)


def overlay_dir(img_orig, hue, center):
    from colorsys import hsv_to_rgb
    from PIL import ImageDraw
    from math import cos, sin

    img = img_orig.copy()
    anti_hue = (hue + 0.5) % 1
    rgb = tuple([int(255 * c) for c in hsv_to_rgb(anti_hue, 1, 1)])
    draw = ImageDraw.Draw(img)
    x, y, phi = center
    dx, dy = cos(phi), sin(phi)
    draw.line([x, y, x + dx * 50, y + dy * 50], fill=rgb)
    return img


def find_avg_hue_diff(img_orig, raw_score, dst_hue, center):
    center_x, center_y, r = center
    diff_akku, v_akku = 0.0, 0
    r = r * 1.5
    if r < 4:
        return None
    assert raw_score.size == img_orig.size
    w, h = raw_score.size
    for py in around(center_y, r):
        for px in around(center_x, r):
            if px < 0 or px >= w or py < 0 or py >= h:
                # FIXME: Ugh
                continue
            x, y = px - center_x, py - center_y
            if (x * x + y * y) > (r * r):
                continue
            if raw_score.getpixel((px, py)) <= 20:
                continue
            rgb = img_orig.getpixel((px, py))
            is_hue, _, _ = rgb_to_hsv(*[c / 255.0 for c in rgb])
            diff = (is_hue - dst_hue + 0.5) % 1
            diff = diff - 0.5
            assert diff <= hue_tolerance_raw + 1e-3
            diff_akku = diff_akku + diff
            v_akku = v_akku + 1
    if v_akku < 1:
        # That's most definitely an error, so don't let computation continue
        print("  Color correction failed!")
        return None
    return diff_akku / v_akku


# ===== Detector class =====

class BlobDetector:
    # Note that x and y are on the image, so axes are "→x" and "↓y",
    # so angles start as → and go towards ↓

    # 'output' members: hue, raw_center, raw_angle
    # 'visualize' members: raw_center_overlay, raw_dot_overlay, raw_dir_overlay

    def __init__(self, hue):
        self.hue = hue

    def analyze(self, img_in):
        # First, find out which pixels are relevant
        self.raw_scores = pixelwise(img_in, score_lambda(self.hue, hue_tolerance_raw))
        self.raw_center = find_center(self.raw_scores)
        if self.raw_center is None:
            return
        if visualize:
            self.raw_center_overlay = overlay_center(img_in, hue, self.raw_center)

        hue_offset = find_avg_hue_diff(img_in, self.raw_scores, self.hue, self.raw_center)
        if hue_offset is None:
            return
        self.cc_hue = (self.hue + hue_offset + 1) % 1

        # FIXME: Only around center!
        self.cc_scores = pixelwise(img_in, score_lambda(self.cc_hue, hue_tolerance_cc))
        self.cc_center = find_center(self.cc_scores)
        if self.cc_center is None:
            return
        if visualize:
            self.cc_center_overlay = overlay_center(img_in, self.cc_hue, self.cc_center)

        print("  Detected hue {} instead.".format(self.cc_hue))
        self.whiteish = pixelwise(img_in, whiteish)
        self.raw_dot = find_dot(self.whiteish, self.raw_center)
        if self.raw_dot is None:
            return
        dx, dy = self.raw_dot
        self.raw_angle = atan2(dy, dx)
        if visualize:
            px, py, _ = self.raw_center
            self.raw_dir_overlay = overlay_dir(self.raw_center_overlay, hue, [px, py, self.raw_angle])
            px, py = px + dx, py + dy
            self.raw_dot_overlay = overlay_center(img_in, hue, [px, py, 5])


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
        # Available:
        # 'whiteish'
        # 'raw_scores', 'raw_center_overlay'
        # 'cc_scores', 'cc_center_overlay'
        # 'raw_dir_overlay', 'raw_dot_overlay'
        for attr in ['raw_scores', 'raw_center_overlay', 'cc_scores', 'cc_center_overlay']:
            try:
                it = bd.__getattribute__(attr)
            except AttributeError:
                continue
            if it is None:
                continue
            with open('analysis_{}_{}.png'.format(hue, attr), 'wb') as out_file:
                it.save(out_file, 'png')
    print("Done!")
