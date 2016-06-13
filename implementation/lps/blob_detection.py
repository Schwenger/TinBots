#!/usr/bin/env python3
# -*- coding:utf-8 -*-

import pyximport
pyximport.install()

from PIL import Image

from detector import BlobDetector


# ===== Example call =====

bot_hues = [0.55, 0.60, 0.65, 0.85, 0.90, 0.95]  # on the range [0, 1]

if __name__ == '__main__':
    from sys import argv

    if len(argv) > 2:
        print("Usage: ./blob_detection.py [path/to/input.png]")
        exit(1)
    if len(argv) == 2:
        img = Image.open(argv[1])
    else:
        img = Image.open('../../heavy/cbp_tests/gen_320x240/PICT0001.JPG.png')
    for hue in bot_hues:
        print("Running with hue {} ...".format(hue))
        bd = BlobDetector(hue)
        bd.analyze(img)
        # Available:
        # 'rough_center_overlay', 'center_overlay', 'dot_overlay', 'dir_overlay'
        for attr in ['rough_center_overlay', 'center_overlay', 'dot_overlay', 'dir_overlay']:
            try:
                it = bd.__getattribute__(attr)
            except AttributeError:
                continue
            if it is None:
                continue
            with open('analysis_{}_{}.png'.format(hue, attr), 'wb') as out_file:
                it.save(out_file, 'png')
    print("Done!")
