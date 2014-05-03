#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <map>
#include <vector>
#include <fstream>
#include <string>
#include "types.h"

using std::cin;
using std::cout;
using std::endl;
using std::map;
using std::vector;
using std::ifstream;
using std::string;

// Functions to parse data
shikaku_input parse_input_file(char* filename);

// Functions for testing
int test_correctness(int **board, int b_size, map<int, int> result);
int get_contiguous_count(int **board, int **status, int i, int j, int b_size);
void print_board(int **board, int b_size);
void print_results(map<int,int> results);
void print_input(shikaku_input in);
void deallocate_shikaku_result(shikaku_result* result);
