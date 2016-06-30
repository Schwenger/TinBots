#!/usr/bin/env python3
# -*- coding:utf-8 -*-

import colorsys
import math
import threading

import numpy

from matplotlib import colors

from PIL import Image, ImageDraw, ImageEnhance

try:
    import picamera.array
    import picamera.camera
except ImportError:
    print('Waring: Unable to import PiCamera.')

from lps.event import Event

HUES = (0.95, 0.54)


class Analyzer:
    def __init__(self, width=800, height=600):
        self.width = width
        self.height = height
        self.y_grid, self.x_grid = numpy.mgrid[0:height:1, 0:width:1]

    def prepare(self, saturation, value):
        return (saturation > 0.5) & (value > 50)

    def analyze(self, hue, saturation, value, target, prepared=None):
        if prepared is None:
            prepared = self.prepare(saturation, value)

        mask = (numpy.abs((hue - target + 0.5) % 1 - 0.5) < 0.1) & prepared
        total = numpy.sum(mask)

        if total < 1:
            return -1, -1, 0, 0

        # calculate rough center
        x = numpy.sum(mask * self.x_grid) / total
        y = numpy.sum(mask * self.y_grid) / total
        r = numpy.sqrt(total / numpy.pi)

        # at least 50% of the matched area has to be filled with the right color
        if total / (math.pi * r ** 2) < 0.5:
            return -1, -1, 0, 0

        # ignore outliers
        y_start = int(max(0, y - r * 1.5))
        y_end = int(min(self.height, y + r * 1.5 + 1))

        x_start = int(max(0, x - r * 1.5))
        x_end = int(min(self.width, x + r * 1.5 + 1))

        mask = mask[y_start:y_end:1, x_start:x_end:1]

        x_grid = self.x_grid[y_start:y_end:1, x_start:x_end:1]
        y_grid = self.y_grid[y_start:y_end:1, x_start:x_end:1]

        mask &= numpy.sqrt((x_grid - x) ** 2 + (y_grid - y) ** 2) < r * 1.5

        total = numpy.sum(mask)

        # not matched
        if total < 5:
            return -1, -1, 0, 0

        x = numpy.sum(mask * x_grid) / total
        y = numpy.sum(mask * y_grid) / total
        r = numpy.sqrt(total / numpy.pi)

        # at least 80% of the matched area has to be filled with the right color
        if total / (math.pi * r ** 2) < 0.8:
            return -1, -1, 0, 0

        # calculate position of white dot
        y_start = int(max(0, y - r * 1))
        y_end = int(min(self.height, y + r * 1 + 1))

        x_start = int(max(0, x - r * 1))
        x_end = int(min(self.width, x + r * 1 + 1))

        x_dot_grid = self.x_grid[y_start:y_end:1, x_start:x_end:1]
        y_dot_grid = self.y_grid[y_start:y_end:1, x_start:x_end:1]

        grid_mask = numpy.sqrt((x_dot_grid - x) ** 2 + (y_dot_grid - y) ** 2) < r * 0.9

        saturation = saturation[y_start:y_end:1, x_start:x_end:1]
        value = value[y_start:y_end:1, x_start:x_end:1]

        inside = ((saturation < 0.5) & (value > 0.5) & grid_mask)

        total = numpy.sum(inside)

        # not matched
        if total < 3:
            return -1, -1, 0, 0

        dot_x = numpy.sum(inside * x_dot_grid) / total
        dot_y = numpy.sum(inside * y_dot_grid) / total

        # calculate heading direction
        angle = math.atan2(dot_y - y, dot_x - x)

        return x, y, angle, r


def render(image, hue, x, y, angle, r):
    draw = ImageDraw.Draw(image)
    color = tuple([int(value * 255) for value in colorsys.hsv_to_rgb(hue, 1, 1)])
    draw.ellipse([x - r, y - r, x + r, y + r], fill=None, outline=color)
    draw.line([x, y, x + r * math.cos(angle), y + r * math.sin(angle)], fill=color)


class Detector(threading.Thread):
    def __init__(self):
        super().__init__()
        self.new_data = Event()

    def run(self):
        with picamera.camera.PiCamera() as camera:
            camera.resolution = (640, 480)
            camera.start_preview()

            analyzer = Analyzer(640, 480)

            while True:
                with picamera.array.PiRGBArray(camera) as output:
                    camera.capture(output, 'rgb')
                    image = output.array

                    output = Image.fromarray(numpy.uint8(image))

                    image = colors.rgb_to_hsv(image)

                    hue = image[:, :, 0]
                    saturation = image[:, :, 1]
                    value = image[:, :, 2]

                    converter = ImageEnhance.Color(output)
                    output = converter.enhance(0)

                    positions = {}

                    for target in HUES:
                        x, y, angle, r = analyzer.analyze(hue, saturation, value, target)
                        if r > 0:
                            positions[target] = {
                                'x': x / 5.8,
                                'y': (480 - y) / 5.8,
                                'phi': abs((angle + math.pi) % (2 * math.pi) - math.pi)
                            }
                            render(output, target, x, y, angle, r)

                    self.new_data.fire(output, positions)

            time.sleep(2)


def main():
    from scipy import ndimage

    from matplotlib import colors

    image = ndimage.imread('../../../heavy/raspberry.jpg')
    image = colors.rgb_to_hsv(image)

    height = image.shape[0]
    width = image.shape[1]

    hue = image[:, :, 0]
    saturation = image[:, :, 1]
    value = image[:, :, 2]

    analyzer = Analyzer(width, height)

    from PIL import Image, ImageEnhance

    output = Image.open('../../../heavy/raspberry.jpg')
    # converter = ImageEnhance.Color(output)
    # output = converter.enhance(0)

    prepared = analyzer.prepare(saturation, value)

    for target in (0.55, 0.60, 0.65, 0.85, 0.90, 0.95):
        x, y, angle, r = analyzer.analyze(hue, saturation, value, target, prepared)
        if r != 0:
            render(output, target, x, y, angle, r)

    output.save('output.png')


if __name__ == '__main__':
    main()
