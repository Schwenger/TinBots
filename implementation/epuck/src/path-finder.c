#include <assert.h>
#include <math.h>

#include "astar/astar.h"

#include "path-finder.h"
#include "pi.h"

typedef struct Context {
	Map *map;
	Position goal;
} Context;

enum {
    PF_inactive,
    PF_running,
    PF_complete
};

extern const Position INVALID_POS; /* sigh. */
const Position INVALID_POS = {-1, -1};

static int end_of_path_p(Position pos);
static int invalid_pos(Position pos, Map *map);

static Position map_discretize(double x, double y) {
    Position res;
    res.x = (int) (floor(x));
    res.y = (int) (floor(y));
    return res;
}

void pf_reset(PathFinderState* pf) {
    pf->locals.state = PF_inactive;
    pf->next = INVALID_POS;
    pf->no_path = 0;
    pf->locals.path_index = 0;
    pf->locals.path[0] = INVALID_POS;
    pf->path_completed = 0;
    pf->drive = 0;
    pf->backwards = 0;
}

static void pathing_failed(PathFinderState* pf) {
    pf->locals.state = PF_complete;
    pf->no_path = 1;
}

static void compute_path(PathFinderInputs* inputs, PathFinderState* pf, Sensors* sens) {
    Position dest, pos;
    int success;
    pf->locals.state = PF_running;
    dest = map_discretize(inputs->dest_x, inputs->dest_y);
    pos = map_discretize(sens->current.x, sens->current.y);
    if (invalid_pos(pos, inputs->map)) {
        /* Uhh */
        pathing_failed(pf);
        return;
    }
    pf->locals.path_index = -1;
    success = pf_find_path(pos, dest, inputs->map, pf->locals.path);
    if (!success) {
        pathing_failed(pf);
    }
}

void pf_step(PathFinderInputs* inputs, PathFinderState* pf, Sensors* sens) {
    switch(pf->locals.state) {
        case PF_inactive:
            if (inputs->compute) {
               compute_path(inputs, pf, sens);
            }
            break;
        case PF_running:
            pf->drive = 1;
            assert(!inputs->step_complete || !inputs->step_see_obstacle);
            if (inputs->step_complete || pf->locals.path_index < 0) {
                Position next_wp;
                next_wp = pf->locals.path[pf->locals.path_index];
                pf->drive = 0;
                pf->backwards = 0;
                /* Q: Why not check here for the position again?  Like this:
                 *    if(pos.x == pf->next.x && pos.y == pf->next.y)
                 * A: What do you if that fails?
                 *      (There's nothing meaningful one could do!)
                 *    Why would this happen anyway?
                 *      (PathExec already checks for stray!)
                 */
                pf->locals.path_index++;
                assert(pf->locals.path_index >= 0);
                next_wp = pf->locals.path[pf->locals.path_index];
                if (end_of_path_p(next_wp)) {
                    pf->locals.state = PF_complete;
                    pf->path_completed = 1;
                } else if (end_of_path_p(pf->locals.path[pf->locals.path_index + 1])) {
                    pf->backwards = inputs->is_victim;
                }
                pf->next = next_wp;
            }
            if (inputs->step_see_obstacle) {
                /* We ran into an obstacle -> internal map must be outdated. */
                compute_path(inputs, pf, sens);
                pf->drive = 0;
            }
            break;
        case PF_complete:
            /* Nothing to do here. */
            break;
        default:
            hal_print("PathFinder illegal state?!  Resetting ...");
            assert(0);
            pf_reset(pf);
            break;
    }
}

static int end_of_path_p(Position pos) {
    return pos.x == INVALID_POS.x && pos.y == INVALID_POS.y;
}

static void find_neighbours(ASNeighborList neighbours, void *node, void *map);
static float heuristic(void *node, void *goal, void *map);
static int search_node_comparator(void *node1, void *node2, void *map);
static Position intersection(Position origin, Position goal, Map *map);

static int invalid_pos(Position pos, Map *map) {
    return pos.x >= map_get_width(map) || pos.x < 0 || pos.y >= map_get_height(map) || pos.y < 0;
}
static int occupied(Position *pos, Map *map) {
    return map_get_field(map, pos->x, pos->y) == FIELD_WALL;
}

static const ASPathNodeSource path_node_source = {
        sizeof(Position),
        &find_neighbours,
        &heuristic,
        NULL,
        &search_node_comparator
};

int pf_find_path(Position position, Position goal, Map *map, Position *path) {
    ASPath  as_path;
    size_t path_length, i;
    Context context;

    context.map = map;
    context.goal = goal;

    as_path = ASPathCreate(&path_node_source, &context, &position, &goal);
    path_length = ASPathGetCount(as_path);

	for(i = 0; i < path_length; ++i) {
        assert(i < MAX_PATH_LENGTH);
		path[i] = *(Position *) ASPathGetNode(as_path, i);
	}
    assert(i <= MAX_PATH_LENGTH);
	path[i] = INVALID_POS;

	ASPathDestroy(as_path);

    return 1; /* FIXME: What happens if A* fails? */
}

#define MAX_NEIGHBOURS 8
static const double theta_range = 2 * M_PI * (1.0 / MAX_NEIGHBOURS);

/**
 * For static upper bounds on the memory consumption we guarantee that the branching factor is no greater than 5
 */
static void find_neighbours(ASNeighborList neighbours, void *node, void *context_p) {
    double thetas[MAX_NEIGHBOURS];
    double cand_next_x, cand_next_y;
    double offset;
    float sl_dist_to_goal;
    int i;
    Position next, cand_next;

	Position *state = (Position *) node;
	Context *context = (Context *) context_p;

    sl_dist_to_goal = heuristic(node, &context->goal, context->map);
	if(sl_dist_to_goal < STEP_DISTANCE){
		/* in reach of goal */
		Position special = intersection(*state, context->goal, context->map);
		ASNeighborListAdd(neighbours, &special, sl_dist_to_goal);
	}

    for(i = 0; i < MAX_NEIGHBOURS; ++i) {
        thetas[i] = fmod(rand(), theta_range);
    }

	for(i = 0; i < MAX_NEIGHBOURS; i++){
        offset = i * theta_range;
		cand_next_x = state->x + STEP_DISTANCE * cos(thetas[i] + offset);
		cand_next_y = state->y + STEP_DISTANCE * sin(thetas[i] + offset);
        cand_next.x = (int) (floor(cand_next_x));
        cand_next.y = (int) (floor(cand_next_y));
		next = intersection(*state, cand_next, context->map);
        sl_dist_to_goal = heuristic(node, &next, NULL);
        ASNeighborListAdd(neighbours, &next, sl_dist_to_goal);
	}
}

static float heuristic(void *node, void *goal_node, void *context) {
    Position *state = (Position *) node;
    Position *goal = (Position *) goal_node;

	int diff_x = state->x - goal->x;
	int diff_y = state->y - goal->y;

    float h = (float) sqrt(diff_x * diff_x + diff_y * diff_y);
    (void)context; /* We don't actually use the context. */
	return h;
}

static int search_node_comparator(void *node1, void *node2, void *context) {
    Position p1 = *(Position *) node1;
    Position p2 = *(Position *) node2;

    int cmp_x = (p1.x > p2.x) - (p1.x < p2.x);
    int cmp_y = (p1.y > p2.y) - (p1.y < p2.y);
    (void)context; /* We don't actually use the context. */
	return cmp_x != 0 ? cmp_x : cmp_y;
}

static Position intersection(Position origin, Position goal, Map* map) {
	int delta_x = (goal.x > origin.x) - (goal.x < origin.x);
	int delta_y = (goal.y > origin.y) - (goal.y < origin.y);


	Position pos = origin;
	Position old = origin;
	while(pos.x != goal.x || pos.y != goal.y) {

		int x = pos.x, y = pos.y;
		if(abs(goal.x - pos.x) > abs(goal.y - pos.y))
			x += delta_x;
		else
			y += delta_y;

		pos.x = x;
        pos.y = y;
		if(invalid_pos(pos, map))
			return old;
		if(occupied(&pos, map))
			return old;

		old = pos;
	}
	return goal;
}
