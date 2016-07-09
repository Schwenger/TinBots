#include <assert.h>
#include <stdio.h>
#include <stdlib.h> /* malloc, free */

#include "path-finder.h"

int main(void){
	const int size = 10;
	int i, j;
    int success;
	int** occ;
    Position start, goal;
    Map map;
    Position* path;

    /* Build a map */
    occ = (int**) malloc(size * sizeof(int*));
	for(i = 0; i < size; i++) {
		occ[i] = (int*) malloc(size * sizeof(int));
		for(j = 0; j < size; ++j)
			occ[i][j] = 0;
	}
	for(i = 0; i < 8; ++i)
		occ[3][i] = 1;

	start.x = 0;
    start.y = 0;
	goal.x = 9;
    goal.y = 9;
	map.width = size;
    map.height = size;
    map.occupancy = occ;

	printf("Starting search\n");
	path = (Position*) malloc((MAX_PATH_LENGTH + 1) * sizeof(Position));
	success = pf_find_path(start, goal, &map, path);
    assert(success);

    printf("Found path:\n(%d, %d)", start.x, start.y);
    for (i = 0; i < MAX_PATH_LENGTH && path[i].x != -1 && path[i].y != -1; ++i) {
		printf("-> (%d, %d)", path[i].x, path[i].y);
	}
    printf("\nAdditional data:\n");
    for (; i < MAX_PATH_LENGTH; ++i) {
		printf("-> (%d, %d)", path[i].x, path[i].y);
	}
    printf("\n\tEVALUATE BY HAND!\n");

	for(i = 0; i < size; i++) {
		free(occ[i]);
	}
    free(occ);
    free(path);

	return 0;
}
