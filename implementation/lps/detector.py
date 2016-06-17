#!/usr/bin/env python3
# -*- coding:utf-8 -*-

import colorsys
import math

import numpy

from PIL import ImageDraw


class Analyzer:
    def __init__(self, width=800, height=600):
        self.width = width
        self.height = height
        self.y_grid, self.x_grid = numpy.mgrid[0:height:1, 0:width:1]

    def analyze(self, hue, saturation, value, target):
        mask = (numpy.abs((hue - target + 0.5) % 1 - 0.5) < 0.15) & (saturation > 0.3) & (value > 0.1)
        total = numpy.sum(mask)

        x = numpy.sum(mask * self.x_grid) / total
        y = numpy.sum(mask * self.y_grid) / total
        r = numpy.sqrt(total / numpy.pi)

        y_start = int(max(0, y - r * 0.9))
        y_end = int(min(self.height, y + r * 0.9 + 1))

        x_start = int(max(0, x - r * 0.9))
        x_end = int(min(self.width, x + r * 0.9 + 1))

        x_dot_grid = self.x_grid[y_start:y_end:1, x_start:x_end:1]
        y_dot_grid = self.y_grid[y_start:y_end:1, x_start:x_end:1]

        grid_mask = numpy.sqrt((x_dot_grid - x) ** 2 + (y_dot_grid - y) ** 2) < r * 0.9

        saturation = saturation[y_start:y_end:1, x_start:x_end:1]
        value = value[y_start:y_end:1, x_start:x_end:1]

        inside = ((saturation < 0.2) & (value > 0.8) & grid_mask)

        total = numpy.sum(inside)

        dot_x = numpy.sum(inside * x_dot_grid) / total
        dot_y = numpy.sum(inside * y_dot_grid) / total

        angle = math.atan2(dot_y - y, dot_x - x)

        return x, y, angle, r


def render(image, hue, x, y, angle, r):
    draw = ImageDraw.Draw(image)
    color = tuple([int(value * 255) for value in colorsys.hsv_to_rgb(hue, 1, 1)])
    draw.ellipse([x - r, y - r, x + r, y + r], fill=None, outline=color)
    draw.line([x, y, x + r * math.cos(angle), y + r * math.sin(angle)], fill=color)


def main():
    from scipy import ndimage

    from matplotlib import colors

    image = ndimage.imread('../../heavy/cbp_tests/gen_800x600/PICT0002.JPG.png')
    image = colors.rgb_to_hsv(image)

    height = image.shape[0]
    width = image.shape[1]

    hue = image[:, :, 0]
    saturation = image[:, :, 1]
    value = image[:, :, 2]

    analyzer = Analyzer(width, height)

    from PIL import Image, ImageEnhance

    output = Image.open('../../heavy/cbp_tests/gen_800x600/PICT0002.JPG.png')
    converter = ImageEnhance.Color(output)
    output = converter.enhance(0)

    for target in (0.60, 0.90):
        x, y, angle, r = analyzer.analyze(hue, saturation, value, target)
        render(output, target, x, y, angle, r)

    output.save('output.png')


if __name__ == '__main__':
    main()
