# -*- coding: utf-8 -*-

from colorsys import rgb_to_hsv
from math import atan2, pi, sqrt
from PIL import Image


cdef struct HSV:
    float hue
    float saturation
    float value

ctypedef void (*score_function)(int, int, int, void*)


# ===== Configuration / magic numbers =====

cdef int HUE_TOLERANCE = 64

VISUALIZE = False


cdef HSV get_hue(int red, int green, int blue) nogil:
    cdef float minc, maxc, value, rc, gc, bc
    cdef HSV result;
    minc = red if red < green else green
    minc = blue if blue < minc else minc
    maxc = red if red > green else green
    maxc = blue if blue > maxc else maxc
    result.value = maxc
    if minc == maxc:
        result.hue = 0
        result.saturation = 0
        return result
    result.saturation = (maxc - minc) / maxc
    rc = (maxc - red) / (maxc - minc)
    gc = (maxc - green) / (maxc - minc)
    bc = (maxc - blue) / (maxc - minc)
    if maxc == red:
        result.hue = bc - gc
    elif maxc == green:
        result.hue = 2.0 + rc - bc
    else:
        result.hue = 4.0 + gc - rc
    result.hue = (result.hue / 6.0) % 1.0
    return result




# ===== Basic building blocks =====

cdef int score_pixel(rgb, x, y, float dst_hue):
    cdef int red, green, blue
    red, green, blue = rgb
    cdef HSV hsv
    cdef float diff
    with nogil:
        hsv = get_hue(red, green, blue)
        diff = dst_hue - hsv.hue
        if diff < 0:
            diff *= -1
        diff %= 1
        if diff > 0.5:
            diff = 1 - diff
        diff *= 256 * 2
        if diff > HUE_TOLERANCE:
            diff = 255
        # These threshold don't need any modification:
        if hsv.saturation < 0.3 or hsv.value < 0.1:
            diff = 255
        return diff <= 128


def score_lambda(dst_hue):
    def score_internal(x, y, rgb):
        # FIXME: Use bind or something?
        return score_pixel(rgb, x, y, dst_hue)
    return score_internal


def whiteish(x, y, rgb):
    _, s, v = rgb_to_hsv(*[x / 255.0 for x in rgb])
    return s < 0.3 and v > 0.8


def around(center, axis, maximum):
    if center is None:
        return (0, maximum)
    radius = center[2]
    center = center[axis]
    begin, end = int(center - radius), int(center + radius) + 1
    begin = min(maximum, max(0, begin))
    end = min(maximum, max(0, end))
    return (begin, end)


def weighted_average(img, fn, center=None):
    w, h = img.size
    cdef int x0, x1, y0, y1, py, px
    x0, x1 = around(center, 0, w)
    y0, y1 = around(center, 1, h)

    x_akku, y_akku, v_akku = 0.0, 0.0, 0
    max_r = None
    if center is not None:
        max_r_sq = center[2] * center[2]
    for py in range(y0, y1):
        for px in range(x0, x1):
            assert px >= 0 and px < w and py >= 0 and py < h
            if center is not None:
                x, y = px - center[0], py - center[1]
                if x * x + y * y > max_r_sq:
                    continue
            if fn(px, py, img.getpixel((px, py))):
                x_akku = x_akku + px
                y_akku = y_akku + py
                v_akku = v_akku + 1
    if v_akku < 1:
        return None
    theoretical_radius = sqrt(v_akku / pi)
    return (x_akku / v_akku, y_akku / v_akku, theoretical_radius)


def tweak_radius(center, factor):
    x, y, r = center
    return (x, y, r * factor)


# ===== Visualization-only stuff =====

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


# ===== Detector class =====

class BlobDetector:
    # Note that x and y are on the image, so axes are "→x" and "↓y",
    # so angles start as → and go towards ↓

    # 'output' members: center, angle
    # 'visualize' members: rough_center_overlay, center_overlay,
    #                      dot_overlay, dir_overlay

    def __init__(self, hue):
        self.hue = hue

    def analyze(self, img_in):
        # First, find out where, roughly, the center is.
        # (Skew comes from false positives)
        rough_center = weighted_average(img_in, score_lambda(self.hue))
        if rough_center is None:
            return
        if VISUALIZE:
            self.rough_center_overlay = overlay_center(img_in, self.hue,
                                                       rough_center)

        # Use the approximate location to find the exact location.
        self.center = weighted_average(img_in, score_lambda(self.hue),
                                       tweak_radius(rough_center, 1.5))
        if self.center is None:
            return
        if VISUALIZE:
            self.center_overlay = overlay_center(img_in, self.hue, self.center)

        # With the exact center, determine where the white "dot" is:
        self.dot = weighted_average(img_in, whiteish,
                                    tweak_radius(rough_center, 0.9))
        if self.dot is None:
            return
        dot_radius = self.dot[2]
        self.dot = self.dot[0:2]  # Kick 'radius'

        # Compute angle:
        dx, dy = self.dot
        dx, dy = dx - self.center[0], dy - self.center[1]
        self.angle = atan2(dy, dx)
        if VISUALIZE:
            self.dot_overlay = overlay_center(img_in, self.hue,
                                              [self.dot[0], self.dot[1], 5])
            self.dir_overlay = overlay_dir(self.center_overlay, self.hue,
                                           [self.center[0], self.center[1],
                                            self.angle])