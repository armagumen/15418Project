#ifndef _TYPES
#define _TYPES

#include <vector>
#include <map>
using std::vector;
using std::map;

// shikaku_req: represents a board requirement (initial numbers on the board)
typedef struct {
	int x;
	int y;
	int value;
}shikaku_req;

// shikaku_input: input used for the solver, includes board dimensions and req
typedef struct{
	int bwidth;
	int bheight;
	vector<shikaku_req> requirements;
}shikaku_input;

// shikaku_results: includes a filled board, and a map with  ids as keys, and their
// amount of squares filled as a value
typedef struct {
	int **board;
	int bwidth;
	int bheight;
	map<int, int> ids;
}shikaku_result;

#endif
