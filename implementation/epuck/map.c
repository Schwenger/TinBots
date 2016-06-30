#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include "map.h"

typedef unsigned char byte;
void serialize(int* data, byte* buffer, int len);
void deserialize(int* buffer, byte* data, int num_bytes);

static const int BIT_PER_FIELD = 2;
static const int EOF_FLAG = 3;
#define TIN_PACKAGE_MAX_LENGTH 128

void serialize(int* data, byte* buffer, int num_fields) {
    byte value, next_value;
    int pos_in_byte = 0;
    int field_cnt = 0;
    int num_bytes = num_fields / BIT_PER_FIELD;
    int fields_per_byte = 8 / BIT_PER_FIELD;
    int i;

    assert(num_bytes < TIN_PACKAGE_MAX_LENGTH);

    for(i = 0; i < num_bytes; ++i) {
        value = 0;
        for(pos_in_byte = 0; pos_in_byte < fields_per_byte; ++pos_in_byte, ++field_cnt) {
            if(field_cnt < num_fields)
                next_value = (byte) data[field_cnt];
            else
                next_value = EOF_FLAG;
            assert(field_cnt >= num_fields || data[field_cnt] >= 0 && data[field_cnt] < (1 << BIT_PER_FIELD));
            next_value <<= pos_in_byte * BIT_PER_FIELD;
            value |= next_value;
            /* printf("value after taking pos %d into consideration: %d\n", pos_in_byte, value); */
        }
        buffer[i] = value;
    }
}

/*
 * the buffer's length has to be GREATER than the number of bytes!!
 */
void deserialize(int* buffer, byte* data, int num_bytes) {
    int byte_cnt, pos, pos_bar, field_cnt;
    byte current, value;
    int fields_per_byte = 8 / BIT_PER_FIELD;

    field_cnt = 0;
    for(byte_cnt = 0; byte_cnt < num_bytes; ++byte_cnt) {
        current = data[byte_cnt];
        /* printf("field_cnt: %d, byte_cnt: %d, fields_per_byte: %d\n", field_cnt, byte_cnt, fields_per_byte); */
        assert(field_cnt == byte_cnt * fields_per_byte);
        for(pos = 0; pos < fields_per_byte; ++pos) {
            pos_bar = (fields_per_byte-1) - pos; /* inverse of pos wrt fields per bytes, i.e. first field is rightmost*/
            value = current;
            value <<= pos_bar * BIT_PER_FIELD;
            value >>= (pos_bar * BIT_PER_FIELD) + (pos * BIT_PER_FIELD);
            /* printf("Value is: %d\n", value); */
            if(value == EOF_FLAG)
                break;
            buffer[field_cnt] = value;
            field_cnt++;
        }
    }
}

void send_map(Map* m, Position center, int radius) {
    int diameter = 2 * radius;
    int fields_to_transmit = diameter * diameter;
    int bytes_needed = fields_to_transmit * BIT_PER_FIELD / 8;
    int x, y;
    int** map = m->occupancy;
    int occ[TIN_PACKAGE_MAX_LENGTH];
    byte data[TIN_PACKAGE_MAX_LENGTH];

    for(x = 0; x < diameter; ++x) {
        for(y = 0; y < diameter; ++y) {
            occ[x * diameter + y] = map[center.x - radius + x][center.y - radius + y];
        }
    }
    assert(bytes_needed < TIN_PACKAGE_MAX_LENGTH);
    serialize(occ, data, fields_to_transmit);
}

#ifdef MAP_TEST
int main(void) {
    const int num_fields = 5;
    const int num_bin_bytes = 2;
    int data[num_fields];
    byte buffer[num_bin_bytes];
    data[0] = 1;
    data[1] = 1;
    data[2] = 2;
    data[3] = 0;
    data[4] = 1;
    buffer[0] = CHAR_MAX;
    buffer[1] = CHAR_MAX;

    serialize(data, buffer, num_fields);
    printf("Value: %d%d\n", buffer[0], buffer[1]);
    printf("Hex: %02x%02x\n", buffer[0], buffer[1]);
    deserialize(data, buffer, num_bin_bytes);
    printf("Value: %d,%d,%d,%d,%d\n", data[0], data[1], data[2], data[3], data[4]);
}
#endif

