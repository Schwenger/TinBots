
#include "map.h"
#include <assert.h>
#include "bluetooth.h"

static const int EOF_FLAG = 3;


void map_serialize(int* data, byte* buffer, unsigned int num_fields) {
    byte value, next_value;
    int pos_in_byte = 0;
    unsigned int field_cnt = 0;
    unsigned int num_bytes = num_fields / BIT_PER_FIELD;
    int fields_per_byte = 8 / BIT_PER_FIELD;
    unsigned int i;

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
void map_deserialize(int* buffer, byte* data, unsigned int num_bytes) {
    unsigned int byte_cnt, field_cnt, pos, pos_bar;
    byte current, value;
    unsigned int fields_per_byte = 8 / BIT_PER_FIELD;

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
