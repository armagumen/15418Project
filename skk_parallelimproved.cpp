#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstddef>
#include <iostream>
#include <omp.h>
#include <vector>
#include <set>
#include <iterator>
#include <algorithm>
#include "CycleTimer.h"

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::set;
using std::string;


typedef struct{
	int x1;
	int y1;
	int x2;
	int y2;
} rectangle;

typedef struct{
	int x;
	int y;
} point;

bool operator<(const point& first, const point& second){
	return (first.x + 1000 * first.y) < (second.x + 1000 * second.y);
}

#define _BWIDTH 6
#define _BHEIGHT 6
static int board[6][6] = {{0}};
static int cellboard[6][6] = {{0}};
static int pval[] = {4,2,9,10,5,6};
static int xval[] = {0,2,1,4,3,5};
static int yval[] = {0,0,2,3,5,5};
static int width = _BWIDTH;
static int height = _BHEIGHT;
static int pp = 6;

static vector< vector<rectangle> > rects; // input/size numbers per processor
static vector<int> ids;
static vector<int> filled;

void initBoard(){
    #pragma omp parallel for
	for(int i = 0; i < sizeof(xval) / 4; i++){
		//board[yval[i]][xval[i]] = 1;
		board[yval[i]][xval[i]] = i+1;
	}

}


void printBoard(){
	for(int j = 0;j < height; j++){
		for(int i = 0; i < width; i++){
			printf("%d ",board[j][i]);	
		}
		printf("\n");
		//do something about printing
		
	}

}

vector<rectangle> getRects(int x, int y, int p){
	vector<rectangle> rects;
	for (int i = 1; i < p+1;i++){
		if (p %i == 0){
			int j = p / i;
			for (int p1 = 0; p1 < i; p1++){
				for (int p2 = 0; p2 < j; p2++){
					int one = x - p1;
					int two = y - p2;
					int three = x + (i - p1) - 1;
					int four = y + (j - p2) - 1;
					if (one >= 0 && two >= 0 && three < width 
						&& four < height){
						rectangle newrect;
						newrect.x1 = one;
						newrect.y1 = two;
						newrect.x2 = three;
						newrect.y2 = four;
						rects.push_back(newrect);
					}
				}
			}
		}
	}
	return rects;
}

void getValid(int num, vector<rectangle>* rects){

	vector<rectangle>::iterator itt = rects->begin();
	vector<int>::iterator ids = ids->begin();

	for(; itt != rects->end(); itt++, ids++){
		int check = 1;
		for(int j = (*itt).y1; j < (*itt).y2+1;j++){
			for(int i = (*itt).x1; i < (*itt).x2+1; i++){

				if(board[j][i] != num && board[j][i] != 0){
					check = 0;
				}

			}
		}
		if(check == 0){
			itt = rects->erase(itt);
			itt--;
		}


	}

}

void fillUp(int num, rectangle r){
	for (int j = r.y1; j < r.y2+1;j++ ){
		for (int i = r.x1; i < r.x2+1;i++){
			board[j][i] = num;
		}
	}

}

void fillSet(set<point> res, int num){
	std::set<point>::iterator it1 = res.begin();
	for(; it1 != res.end();it1++){
		board[(*it1).y][(*it1).x] = num;
	}
}

void mustFill(int index){
	vector<rectangle>::iterator itt = rects[index].begin();
	int start = 0;
	set<point> comp;
	set<point> res;

	for(int k = 0; k < rects[index].size(); k++){
		rectangle currec = rects[index].at(k);
		start++;
		comp.clear();
		for(int j = currec.y1; j < currec.y2+1;j++){
			for(int i = currec.x1; i < currec.x2+1; i++){
				point np;
				np.x = i;
				np.y = j;
				if(start == 1){	
					res.insert(np);
				}
				else{
					comp.insert(np);
				}
				cellboard[j][i]++;
			}
		}
		if(start != 1){
			std::vector<point> v;
			set_intersection(res.begin(), res.end(), comp.begin(),
						 comp.end(), std::back_inserter(v));
			set<point> ss(v.begin(),v.end());
			res = ss;
		}

	}
	fillSet(res, ids[index]);
}

set<point> getOnly(){
	set<point> temp;
	for(int j = 0;j < height; j++){
		for(int i = 0; i < width; i++){
			if(cellboard[j][i] == 1){
				point pt;
				pt.x = i;
				pt.y = j;
				temp.insert(pt);		
			}
		}
	}
	return temp;
}


void fillUpInd(int num,int x1,int x2,int y1, int y2){
	//rectangle r = rects[num-1].front();
	for (int j = y1; j < y2+1;j++ ){
		for (int i = x1; i < x2+1;i++){
			board[j][i] = num;
		}
	}

}

void onlyFill(int index, set<point> o){
	vector<rectangle>::iterator itt = rects[index].begin();
	set<point> res;
		for(; itt != rects[index.end();itt++){
			for(int j = (*itt).y1; j < (*itt).y2+1;j++){
				for(int i = (*itt).x1; i < (*itt).x2+1; i++){
					point pt; pt.x = i; pt.y = j;
					if(o.count(pt) == 1){
						fillUpInd(ids[index],(*itt).x1,(*itt).x2,
									(*itt).y1, (*itt).y2);
						filled[index] = 1;
						return;				
					}
				}
			}
		}
}

void reset_cellboard(){
	for(int i = 0; i < _BWIDTH; i++){
		for(int j = 0; j < _BHEIGHT; j++){
			cellboard[i][j] = 0;
		}
	}
}

int isAll(){
	for (int j = 0; j < _BHEIGHT;j++){
		for (int i = 0; i < _BWIDTH;i++){
			if(board[j][i] == 0) return 0;
		}
	}
	return 1;
}

int sync_board(int rank, int total_procs){
	// Convert the board to an array and fill it
	int* board_buffer = malloc(sizeof(int) * width * height);
	int* cell_buffer = malloc(sizeof(int) * width * height);
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			board_buffer[(i * width) + j] = board[i][j];
			cell_buffer[(i * width) + j] = cellboard[i][j];
		}
	}

	// Allocate a buffer for all the elements
	int total_rcv_count = width * height * total_procs;
	int* all_buffer = malloc(sizeof(int) * total_rcv_count);

	// All to all the boards
	MPI_Alltoall(board_buffer, width * height, MPI_INT, all_buffer, 
		width * height, MPI_INT, MPI_COMM_WORLD);

	// Create the new board
	for(int p = 0; p < total_procs; p++){
		for(int i = 0; i < height; i++){
			for(int j = 0; j < width; j++){
				if(p == 0){ // Just copy the first ones
					board[i][j] = all_buffer[(p * width * height) + (i * width + j)];
				}
				else{ // Then add up the following
					board[i][j] += all_buffer[(p * width * height) + (i * width + j)];
				}

			}
		}
	}

	// All to all the cellboards
	MPI_Alltoall(cell_buffer, width * height, MPI_INT, all_buffer, 
		width * height, MPI_INT, MPI_COMM_WORLD);

	// Create the new cellboard
	for(int p = 0; p < total_procs; p++){
		for(int i = 0; i < height; i++){
			for(int j = 0; j < width; j++){
				if(p == 0){ // Just copy the first ones
					cellboard[i][j] = all_buffer[(p * width * height) + (i * width + j)];
				}
				else{ // Then add up the following
					cellboard[i][j] += all_buffer[(p * width * height) + (i * width + j)];
				}

			}
		}
	}

	// Free buffer memory
	free(board_buffer);
	free(cell_buffer);
	free(all_buffer);
}

int solve_mpi(){
	int size;  // number of processes
 	int rank;  // current process id
  	int squares_completed  = 0;
  	int total_squares;

  	// MPI Initialization
	MPI_Init(& argc, & argv);
	MPI_Comm_rank(MPI_COMM_WORLD, & rank); // get id of current process
  	MPI_Comm_size(MPI_COMM_WORLD, & size); // get number of processes
  	
  	// Set the ids on the board
  	initBoard();

  	// Make each processor generate it's rectangles, and the IDs it is
  	// responsible for
  	for(int i = 0; i < pp; i+=size){
  		int index = i + rank;
  		if(index < pp){
			rects.push_back(getRects(xval[index],yval[index],pval[index]));
			ids.push_back(index);
			filled.push_back(0);
		}
	}

	total_squares = ids.size();
	while(isAll() == 0){
		reset_cellboard();

		// Get the valid rectangles, for each ID
		for(int i = 0; i < total_squares; i ++){
			if(filled[i] != 1){
				getValid(ids[i],&(rects[i]);
			}
		}	
		
		// If one ID has only one rectangle, fill it up rightaway
		for(int i = 0; i < total_squares; i++){
			if(rects[i].size() == 1 && filled[i] != 1){
				fillUp(ids[i], rects[i]); // Fill the positions of a rectangle, with an id
				filled[i] = 1;
				squares_completed++;
			}
		}
		/*
		 * OPTIMIZATION
		 * Board may have changed, but no need to send yet, since the ones filled
		 * are unique, and nothing interferes with them
		 */
		 
		// Calculate board positions where all rectangles intersect, for a specific ID,
		// and fill them up
		for(int i = 0; i < total_squares; i++){
			if(filled[i] != 1){
				mustFill(i);
			}
		}

		// UPDATE BOARD IN ALL PROCESSES 
		sync_board(rank, size);

		// Remove invalid rectangles, for each ID
		for(int i = 0; i < total_squares; i ++){
			if(filled[i] != 1){
				getValid(ids[i],&(rects[i]);
			}	
		}

		// Check board positions covered by only one rectangle
		set<point> only = getOnly();

		// ... and fill the board with such rectangles
		for(int i = 0; i < total_squares; i++){
			if(filled[i] != 1){
				onlyFill(i, only);

			}
		}

		// UPDATE BOARD IN ALL PROCESSES 
		sync_board(rank, size);
		if(rank == 0){ // executed only by the main process
			printBoard();
  		}
	}
  	MPI_Finalize();
}

int main(){

    double start_time = CycleTimer::currentSeconds();
	
	solve_mpi();

    double end_time = CycleTimer::currentSeconds();
    printf("time: %.16f sec\n", end_time - start_time);



}
