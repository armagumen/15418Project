#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <map>
#include <vector>
#include <fstream>
#include <string>

using std::cin;
using std::cout;
using std::endl;
using std::map;
using std::vector;
using std::ifstream;
using std::string;

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

// Functions to parse data
shikaku_input parse_input_file(char* filename);

// Functions for testing
int test_correctness(int **board, int b_size, map<int, int> result);
int get_contiguous_count(int **board, int **status, int i, int j, int b_size);
void print_board(int **board, int b_size);
void print_results(map<int,int> results);
void print_input(shikaku_input in);
void deallocate_shikaku_result(shikaku_result* result);
