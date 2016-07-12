#!/usr/bin/env python3


def emit_row(y):
    print("""
    dst_data[0 + {y} * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[0 + {y} * DST_CELL_TWO_ROWS_U16S], patch_data[0 + {y} * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[1 + {y} * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[1 + {y} * DST_CELL_TWO_ROWS_U16S], patch_data[1 + {y} * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[2 + {y} * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[2 + {y} * DST_CELL_TWO_ROWS_U16S], patch_data[2 + {y} * PROX_CELL_TWO_ROWS_U16S]);
    dst_data[3 + {y} * DST_CELL_TWO_ROWS_U16S] = merge_u16(
        dst_data[3 + {y} * DST_CELL_TWO_ROWS_U16S], patch_data[3 + {y} * PROX_CELL_TWO_ROWS_U16S]);""".format(y=y))


def emit_all():
    for y in range(8):
        emit_row(y)


if __name__ == '__main__':
    emit_all()
