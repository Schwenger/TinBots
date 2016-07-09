#include <assert.h>
#include <math.h>

#include "hal.h"
#include "pi.h"

#include "t2t.h"
#include "t2t-parse.h"

/* ===== Sending ===== */

enum {
    /* Documentation at https://piratenpad.de/p/ZuCsUfLZOdj0ua */
    T2T_COMMAND_HEARTBEAT = 0x08,
    T2T_COMMAND_FOUND_PHI = 0x0A,
    T2T_COMMAND_FOUND_XY = 0x0B,
    T2T_COMMAND_UPDATE_MAP = 0x0C,
    T2T_COMMAND_DOCKED = 0x0D,
    T2T_COMMAND_COMPLETED = 0x0E,
    /* Use the reserved numbers so that clang complains in the event of a clash: */
    ZZZ_RESERVED_PRINT = 0x00, /* Also, SCAN */
    ZZZ_RESERVED_HELLO = 0x01,
    ZZZ_RESERVED_PHI_DEPRECATED = 0x02,
    ZZZ_RESERVED_LPS = 0x04,
    ZZZ_RESERVED_XY_DEPRECATED = 0x09,
    ZZZ_RESERVED_SET_LED = 0x80,
    ZZZ_RESERVED_SET_MOTOR = 0x81,
    ZZZ_RESERVED_TX_AMBIENT = 0x82,
    ZZZ_RESERVED_TX_REFLECTION = 0x83
};

void t2t_send_heartbeat(void) {
    hal_send_done(T2T_COMMAND_HEARTBEAT);
}

static void put_uint16(unsigned int i, char* buf) {
    buf[0] = (char)((i >> 8) & 0xFF);
    buf[1] = (char)(i & 0xFF);
}

static char cap_char(int i) {
    i = (i > 255) ? 255 : ((i < 0) ? 0 : i);
    return (char)i;
}

static unsigned int cap_int(long l) {
    l = (l > 65535) ? 65535 : ((l < 0) ? 0 : l);
    return (unsigned int)l;
}

void t2t_send_found_phi(int x, int y, double victim_phi) {
    char buf[4];
    long long_angle;
    buf[0] = cap_char(x);
    buf[1] = cap_char(y);
    victim_phi = fmod(victim_phi, 2 * M_PI);
    if (victim_phi < 0) {
        victim_phi += 2 * M_PI;
    }
    long_angle = (long)(victim_phi * 1000);
    put_uint16(cap_int(long_angle), buf + 2);
    hal_send_put(buf, sizeof(buf));
    hal_send_done(T2T_COMMAND_FOUND_PHI);
}

void t2t_send_found_xy(int x, int y, int iteration) {
    char buf[3];
    buf[0] = cap_char(x);
    buf[1] = cap_char(y);
    buf[2] = cap_char(iteration);
    hal_send_put(buf, sizeof(buf));
    hal_send_done(T2T_COMMAND_FOUND_XY);
}

void t2t_send_update_map(int x, int y, Map* map) {
    char buf[2];
    buf[0] = cap_char(x);
    buf[1] = cap_char(y);
    hal_send_put(buf, sizeof(buf));
    hal_send_put((char*)map_serialize(map), MAP_PROXIMITY_BUF_SIZE);
    hal_send_done(T2T_COMMAND_UPDATE_MAP);
}

void t2t_send_docked(void) {
    hal_send_done(T2T_COMMAND_DOCKED);
}

void t2t_send_completed(void) {
    hal_send_done(T2T_COMMAND_COMPLETED);
}


/* ===== Receiving ===== */

void t2t_parse_heartbeat(TinBot* bot) {
    t2t_receive_heartbeat(bot);
}

static unsigned int parse_uint16(char* buf) {
    unsigned int val = (unsigned int)(buf[2]) << 8;
    val |= (unsigned int)(buf[1]) & 0xFF;
    return val;
}

void t2t_parse_found_phi(TinBot* bot, char* data, unsigned int length) {
    assert(length == 4);
    t2t_receive_found_phi(bot, data[0], data[1], parse_uint16(data + 2));
}

void t2t_parse_found_xy(TinBot* bot, int is_ours, char* data, unsigned int length) {
    assert(length == 3);
    t2t_receive_found_xy(bot, is_ours, data[0], data[1], data[2]);
}

void t2t_parse_update_map(TinBot* bot, int is_ours, char* data, unsigned int length) {
    assert(length == 2 + MAP_PROXIMITY_BUF_SIZE);
    t2t_receive_update_map(bot, is_ours, data[0], data[1], map_deserialize((unsigned char*)data + 2));
}

void t2t_parse_docked(TinBot* bot) {
    t2t_receive_docked(bot);
}

void t2t_parse_completed(TinBot* bot) {
    t2t_receive_completed(bot);
}
