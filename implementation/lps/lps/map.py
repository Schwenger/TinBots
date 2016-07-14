import itertools
import numpy


# Keep the following definitions in sync with map.h.
# The names are identical in order to make the correlation
# unmistakeably clear.
MAP_PROX_SIZE = 16
MAP_MAX_WIDTH = 100
MAP_MAX_HEIGHT = 100

# Possible values for a field
FIELD_UNKNOWN = 0
FIELD_FREE = 1
FIELD_WALL = 2


class Map:
    def __init__(self):
        self.array = numpy.zeros((100, 100), dtype=numpy.uint8)
        # access with: self.array[y][x]

    def update(self, x, y, field):
        if field != 0:
            self.array[y][x] = field

    def patch(self, ll_x, ll_y, data):
        # If the data is bad, don't enter it at all
        assert(0 < ll_x and ll_x + MAP_PROX_SIZE < MAP_MAX_WIDTH)
        assert(0 < ll_y and ll_y + MAP_PROX_SIZE < MAP_MAX_HEIGHT)
        assert(len(data) == MAP_PROX_SIZE * MAP_PROX_SIZE / 4)
        for x, y in itertools.product(
                range(MAP_PROX_SIZE / 2), range(MAP_PROX_SIZE / 4)):
            # First, the byte of the "upper" four fields:
            val = data[0 + x + y * 2 * (MAP_PROX_SIZE / 4)]
            self.update(ll_x + x * 4 + 0, ll_y + y * 2 + 0, (val >> 0) & 0b11)
            self.update(ll_x + x * 4 + 1, ll_y + y * 2 + 0, (val >> 2) & 0b11)
            self.update(ll_x + x * 4 + 2, ll_y + y * 2 + 0, (val >> 4) & 0b11)
            self.update(ll_x + x * 4 + 3, ll_y + y * 2 + 0, (val >> 6) & 0b11)
            # Then, the byte of the "lower" four fields:
            val = data[1 + x + y * 2 * (MAP_PROX_SIZE / 4)]
            self.update(ll_x + x * 4 + 0, ll_y + y * 2 + 1, (val >> 0) & 0b11)
            self.update(ll_x + x * 4 + 1, ll_y + y * 2 + 1, (val >> 2) & 0b11)
            self.update(ll_x + x * 4 + 2, ll_y + y * 2 + 1, (val >> 4) & 0b11)
            self.update(ll_x + x * 4 + 3, ll_y + y * 2 + 1, (val >> 6) & 0b11)
        # Yeah, that's pretty convoluted.  However, it allows for several nice
        # optimizations in map_merge without exploding the alignment
        # properties that need to be obeyed by prox-map.c, function
        # 'desired_position'
