#include <stdio.h>
#include <map.h>
#include <limits.h>
#include <assert.h>

#define num_fields (5)
#define num_bin_bytes (2)

int main(void) {
    int i;
    int data_begin[num_fields] =        {1, 1, 2, 0, 1};
    int data_begin_sanity[num_fields] = {1, 1, 2, 0, 1};
    int data_end[num_fields] = {-1, -1, -1, -1, -1};
    char buffer[num_bin_bytes];
    buffer[0] = CHAR_MAX;
    buffer[1] = CHAR_MAX;

    map_serialize(data_begin, buffer, num_fields);
    for(i = 0; i < num_fields; ++i)
        assert(data_begin_sanity[i] == data_begin[i]);
    map_deserialize(data_end, buffer, num_bin_bytes);
    for(i = 0; i < num_fields; ++i)
        assert(data_begin[i] == data_end[i]);
}
