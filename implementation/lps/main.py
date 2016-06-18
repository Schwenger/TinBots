#!/usr/bin/env python3
# -*- coding:utf-8 -*-

import time

import numpy

import picamera.camera
import picamera.array

from matplotlib import colors

from PIL import Image, ImageEnhance

from detector import Analyzer, render

with picamera.camera.PiCamera() as camera:
    camera.resolution = (640, 480)
    camera.start_preview()

    analyzer = Analyzer(640, 480)

    overlay = None

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

            for target in (0.60, ):
                x, y, angle, r = analyzer.analyze(hue, saturation, value, target)
                if r > 0:
                    render(output, target, x, y, angle, r)

            output.save('output.png')

        time.sleep(5)



