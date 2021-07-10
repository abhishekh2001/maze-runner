#include "main.h"

#ifndef CELL_H
#define CELL_H

#define	up		0
#define	down	1
#define right	2
#define left	3
#define NOT_ANY_DIRECTION 4
#define NOTUP	4
#define NOTDOWN	5
#define NOTRIGHT 6
#define NOTLEFT	7



#define INIT_TIMEFACTOR 20

typedef struct Cell{
	bool is_open;
	bool road[4];

	Cell() {
		is_open = false;
		road[0] = false;
		road[1] = false;
		road[2] = false;
		road[3] = false;
	}
} Cell;

#endif
