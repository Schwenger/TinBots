#include <assert.h>
#include <stdio.h>
#include <stdlib.h> /* malloc, free */

#include "map_heap.h"
#include "path-finder.h"

int main(void){
	const int size = 10;
	int i;
    int success;
    Position start, goal;
    Map* map;
    Position* path;

    /* Build a map */
    map = map_heap_alloc(size, size);
	for(i = 0; i < 8; ++i) {
        map_set_field(map, 3, i, FIELD_WALL);
    }

	start.x = 0;
    start.y = 0;
	goal.x = 9;
    goal.y = 9;

	printf("Starting search\n");
	path = (Position*) malloc((MAX_PATH_LENGTH + 1) * sizeof(Position));
	success = pf_find_path(start, goal, map, path);
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

    free(path);
    map_heap_free(map);

	return 0;
}
