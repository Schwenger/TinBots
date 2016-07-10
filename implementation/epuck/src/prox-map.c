#include <assert.h>
#include <math.h>

#include "prox-map.h"
#include "t2t-parse.h"

/* Invariant: lower_left.x%4==0
 *
 * This allows for an efficient implementation in t2t_receive_update_map. */

static const double HALF_SIZE = MAP_PROXIMITY_SIZE * 0.5;
static const double FLICKER_INERTIA = 1;

static void desired_position(Position* into, Sensors* from) {
    into->x = (int)((from->current.x - HALF_SIZE) / 4 + 0.5) * 4;
    into->y = (int)((from->current.y - HALF_SIZE) + 0.5);
}

void proximity_reset(ProxMapState* prox_map, Sensors* sens) {
    desired_position(&prox_map->lower_left, sens);
    map_clear(map_get_proximity());
}

static void maybe_move(ProxMapState* prox_map, Sensors* sens) {
    Position next;
    if (fabs(prox_map->lower_left.x + HALF_SIZE - sens->current.x) < 2 + FLICKER_INERTIA
        && fabs(prox_map->lower_left.y + HALF_SIZE - sens->current.y) < FLICKER_INERTIA) {
        return;
    }

    /* Need to move. */
    desired_position(&next, sens);
    /* If (for example) our offset increases, the map-data needs to shift
     * towards the negative.  Thus, we swap the signs here and let
     * map_move() beleive that we are moving the map-data (and not our
     * frame of reference). */
    map_move(map_get_proximity(),
        next.x - prox_map->lower_left.x,
        next.y - prox_map->lower_left.y);
}

static void enter_new_data(ProxMapState* prox_map, Sensors* sens) {
    /* FIXME:
     * - if a proximity sensor is on, add a wall (FIELD_WALL)
     * - if a proximity sensor is off, remove all walls for the next 7 cm (FIELD_FREE)
     * - except when there's VICTOR standing in the way.
     *      * You'll need to invent some constant "IR_OPENING_ANGLE" for that.
     * - check whether x/y are legal coordinates with map_get_width(), map_get_height() */
    Map* map = map_get_proximity();
    /* map_set_field(map, x, y, FIELD_FREE); */
}

void proximity_step(ProxMapState* prox_map, Sensors* sens) {
    maybe_move(prox_map, sens);
    enter_new_data(prox_map, sens);
}

void proximity_send(ProxMapState* prox_map) {
    t2t_send_update_map(prox_map->lower_left.x, prox_map->lower_left.y,
        map_get_proximity());
    map_clear(map_get_proximity());
}
