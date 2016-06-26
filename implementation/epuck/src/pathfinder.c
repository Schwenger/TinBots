//
// Created by Maximilian Schwenger on 19/06/16.
//

#include "pathfinder.h"
#include <math.h>
#define PI 3.14159265358979323846

static const ASPathNodeSource path_node_source = {
        sizeof(Position),
        &find_neighbours,
        &heuristic,
        NULL,
        &search_node_comparator
};

Position* find_path(Position position, Position goal, Map *map, Position *path) {
    ASPath as_path = ASPathCreate(&path_node_source, map, &position, &goal);
	int path_length = ASPathGetCount(as_path);

	int i;
	for(i = 0; i < path_length; ++i) {
		path[i] = *(Position *) ASPathGetNode(as_path, i);
	}
	path[i] = NO_PATH;

	ASPathDestroy(as_path);
	return path;
}

double right_angle = PI/2;

/**
 * For static upper bounds on the memory consumption we guarantee that the branching factor is no greater than 4
 */
void find_neighbours(ASNeighborList neighbours, void *node, void *context) {

	Position *state = (Position *) node;
	Map *map = (Map *) context;


	if(heuristic(node, &map->goal, map) < STEP_DISTANCE){
		/* in reach of goal */
		Position special = intersection(*state, map->goal, map);
		ASNeighborListAdd(neighbours, &special, 1);
	}

	double thetas[4];
	thetas[0] = (double) rand() / (double) PI;
	thetas[1] = (double) rand() / (double) PI;
	thetas[2] = (double) rand() / (double) PI;
	thetas[3] = (double) rand() / (double) PI;

	double cand_next_x, cand_next_y;
	Position next;
	Position cand_next;
	int i;
	for(i = 0; i < 4; i++){
		cand_next_x = state->x + STEP_DISTANCE * cos(thetas[i] + i * right_angle);
		cand_next_y = state->y + STEP_DISTANCE * sin(thetas[i] + i * right_angle);
		cand_next = (Position) {(int) cand_next_x, (int) cand_next_y};
		next = intersection(*state, cand_next, map);
		ASNeighborListAdd(neighbours, &next, 1);
	}
}

float heuristic(void *node, void *goal_node, void *map) {
	printf("In heuristic \n");
    Position *state = (Position *) node;
    Position *goal = (Position *) goal_node;

	int diff_x = state->x - goal->x;
	int diff_y = state->y - goal->y;

    float h = (float) sqrt(diff_x * diff_x + diff_y * diff_y);
	return h;
}

int search_node_comparator(void *node1, void *node2, void *map) {
    Position p1 = *(Position *) node1;
    Position p2 = *(Position *) node2;

    int cmp_x = (p1.x > p2.x) - (p1.x < p2.x);
    int cmp_y = (p1.y > p2.y) - (p1.y < p2.y);
	return cmp_x != 0 ? cmp_x : cmp_y;
}

Position intersection(Position origin, Position goal, Map* map) {
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

		pos = (Position) {x, y};
		if(invalid_pos(pos, map))
			return old;
		if(occupied(&pos, map))
			return old;

		old = pos;
	}
	return goal;
}

int main(void){
	printf("Welcome to the A-Puckture Science Enrichment Center.\n");

	const int size = 10;
	int** occ = (int**) malloc(size * sizeof(int*));
	int i, j;
	for(i = 0; i < size; i++) {
		occ[i] = (int *) malloc(size * sizeof(int));
		for(j = 0; j < size; ++j)
			occ[i][j] = 0;
	}

	for(i = 0; i < 8; ++i)
		occ[3][i] = 1;

	Position start = {0, 0};
	Position goal = {9, 9};

	Map map = {.width = size, .height = size, .occupancy = occ, .goal = goal};

	Position* path = malloc(100 * sizeof(Position*));

	printf("starting search\n");

	find_path(start, goal, &map, path);

	i = 0;
	while(path[i].x != -1 && path[i].y != -1) {
		printf("(%d, %d) -> ", path[i].x, path[i].y);
		++i;
	}
	return 0;
}
