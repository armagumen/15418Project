#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <map>
#include "utils/types.h"
#include "utils/testing.h"
#include <string.h>

using std::cin;
using std::cout;
using std::endl;
using std::map;

shikaku_result* solve_shikaku(shikaku_input input);
int main(){
	// Parse a file, to get a shikaku_input
	char* filename = "tests/small_board.test";
	shikaku_input in = parse_input_file(filename); //change this to use argv
	
	// Print the input struct to see whats going on
	print_input(in);

	// Call function to solve shikaku, returns a newly allocated pointer, must free later
	shikaku_result* result = solve_shikaku(in);
	
	// Print board and results
	print_board(result->board, result->bwidth);
	print_results(result->ids);
	
	// Check for correctness
	if(test_correctness(result->board, result->bwidth, result->ids))
		cout<<"Results are correct"<<endl;
	else
		cout<<"Results are NOT correct"<<endl;

	// Deallocate memory
	deallocate_shikaku_result(result);
	return 0;
}

// Placeholder function for solving shikaku, just assigns an arbitrary result to it
shikaku_result* solve_shikaku(shikaku_input input){
	shikaku_result* result = (shikaku_result*)malloc(sizeof(shikaku_result));
	int **board;
	board = (int **) malloc(sizeof(int *) * input.bwidth);
	for(int i = 0; i < input.bwidth; i++){
		board[i] = (int *) malloc(sizeof(int) * input.bheight);
		for(int j = 0; j < input.bheight; j++){
			board[i][j] = i;
		}
	}
	result->bwidth = input.bwidth;
	result->bheight  = input.bheight;
	result->board = board;

	map<int, int> ids_result;
	ids_result[0] = 3;
	ids_result[1] = 3;
	ids_result[2] = 2;
	ids_result[3] = 1;
	board[2][2] = 3;

	result->ids = ids_result;
	
	return result;	
}
