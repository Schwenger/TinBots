import numpy


class Map:
    def __init__(self):
        self.array = numpy.zeros((100, 100), dtype=numpy.uint8)
        # access with: self.array[y][x]
