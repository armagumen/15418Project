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
#include "../utils/CycleTimer.h"
#include <mpi.h>

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
/*
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
*/

/*
#define _BWIDTH 10
#define _BHEIGHT 10
static int board[10][10] = {{0}};
static int cellboard[10][10] = {{0}};
static int pval[] = {7,8,9,5,5,4,6,8,10,9,3,6,5,2,6,8};
static int xval[] = {0,0,1,1,2,2,3,3,6,6,7,7,8,8,9,9};
static int yval[] = {0,9,4,5,1,8,3,6,3,6,1,8,4,5,0,9};
static int width = 10;
static int height = 10;
static int pp = 16;
*/
/*
#define _BWIDTH 10
#define _BHEIGHT 18
static int board[10][18] = {{0}};
static int cellboard[10][18] = {{0}};
static int pval[] = {4,9,6,4,6,9,6,4,6,4,4,2,4,4,8,3,8,4,6,4,3,9,6,4,3,4,6,6,6,4,4,4,2,6,2,6};
static int xval[] = {0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,4,4,4,5,5,5,6,6,6,7,7,7,7,8,8,8,8,9,9,9,9};
static int yval[] = {0,6,8,16,7,9,14,17,1,3,12,15,2,6,13,1,7,9,8,10,16,4,11,15,2,5,14,16,0,3,8,10,1,9,11,17};
static int width = 10;
static int height = 18;
static int pp = 36;
*/
#define _BWIDTH 15
#define _BHEIGHT 15
static int board[15][15] = {{0}};
static int cellboard[15][15] = {{0}};
static int pval[] = {2,4,3,2,2,20,2,2,3,2,5,10,2,2,15,24,4,35,15,28,2,3,4,6,2,5,2,2,10,4,3};
static int xval[] = {0,1,9,11,13,6,12,13,14,0,10,12,3,5,9,13,7,12,0,3,7,1,7,2,0,3,7,0,1,8,12};
static int yval[] = {0,0,0,0,0,1,1,1,2,3,3,3,4,4,4,5,6,7,8,8,8,9,9,10,12,12,13,14,14,14,14};
static int width = 15;
static int height = 15;
static int pp = 31;




static vector<int> changes_x;
static vector<int> changes_y;
static vector<int> changes_id;




static vector< vector<rectangle> > rects; // input/size numbers per processor
static vector<int> ids;
static vector<int> filled;
static vector<int> allids;
void initBoard(){
	for(int i = 0; i < sizeof(xval) / 4; i++){
		//board[yval[i]][xval[i]] = 1;
		board[yval[i]][xval[i]] = i+1;
		point pt;pt.x = xval[i]; pt.y = yval[i];
		allids.push_back(i+1);
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

	for(; itt != rects->end(); itt++){
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
			changes_x.push_back(i);
			changes_y.push_back(j);
			changes_id.push_back(num);
		}
	}

}

void fillSet(set<point> res, int num){
	std::set<point>::iterator it1 = res.begin();
	for(; it1 != res.end();it1++){
		board[(*it1).y][(*it1).x] = num;
		changes_x.push_back((*it1).x);
		changes_y.push_back((*it1).y);
		changes_id.push_back(num);
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
			changes_x.push_back(i);
			changes_y.push_back(j);
			changes_id.push_back(num);
		}
	}

}

void onlyFill(int index, set<point> o){
	vector<rectangle>::iterator itt = rects[index].begin();
	set<point> res;
		for(; itt != rects[index].end();itt++){
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
//static int board_buffer[36];
//static int cell_buffer[36];
//static int all_buffer[72];
static double total_comm = 0;
int sync_board(int rank, int total_procs){
    	double start = CycleTimer::currentSeconds();
	// Allocate space for receiving the counts
	int* rcv_counts = (int*) malloc(sizeof(int) * total_procs);
	
	// Get the amount of updates to send
	int send_count;
	send_count = changes_id.size();
	
	// Gather the update sizes
	MPI_Allgather(&send_count, 1, MPI_INT, rcv_counts, 
		1, MPI_INT, MPI_COMM_WORLD);

	// Allocate the space to receive the updates
	int total_rcv_count = 0;
	for(int i = 0; i < total_procs; i++)
		total_rcv_count += rcv_counts[i];
	
	int* updates_x = (int*) malloc(sizeof(int) * total_rcv_count);
	int* updates_y = (int*) malloc(sizeof(int) * total_rcv_count);
	int* updates_num = (int*) malloc(sizeof(int) * total_rcv_count);
	int* displacements = (int*)malloc(sizeof(int) * total_procs);
	
	int curr_displ = 0;
	for(int i = 0; i < total_procs; i++){
		displacements[i] = curr_displ;
		curr_displ += rcv_counts[i];
	}
	//if(rank == 0){

  	//}


	// Gather the updates
	MPI_Allgatherv(&(changes_x[0]), send_count, MPI_INT, updates_x, 
		rcv_counts, displacements, MPI_INT, MPI_COMM_WORLD);
	MPI_Allgatherv(&(changes_y[0]), send_count, MPI_INT, updates_y, 
		rcv_counts, displacements, MPI_INT, MPI_COMM_WORLD);
	MPI_Allgatherv(&(changes_id[0]), send_count, MPI_INT, updates_num, 
		rcv_counts, displacements, MPI_INT, MPI_COMM_WORLD);
		
	
	// Update the board
	for(int i = 0; i < total_rcv_count; i++){
		board[updates_y[i]][updates_x[i]] = updates_num[i];
	}

	// Free memory
	free(rcv_counts);
	free(updates_x);
	free(updates_y);
	free(updates_num);

/*
	// Convert the cellboard to an array and fill it
	int* cell_buffer = (int*) malloc(sizeof(int) * width * height);
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			cell_buffer[(i * width) + j] = cellboard[i][j];
		}
	}
	// Allocate a buffer for all the elements
	total_rcv_count = width * height * total_procs;
	int* all_buffer = (int*) malloc(sizeof(int) * total_rcv_count);

	// All to all the cellboards
	MPI_Allgather(cell_buffer, width * height, MPI_INT, all_buffer, 
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
	}*/
	changes_x.clear();
	changes_y.clear();
	changes_id.clear();
	//Free buffer memory
	//free(cell_buffer);
	//free(all_buffer);
  if(rank == 0){
    	double end = CycleTimer::currentSeconds();
			total_comm += (end - start);
    	printf("comm time: %.16f sec\n",  total_comm);
  	}
}
#define _PROC 0
int solve_mpi(int *argc, char ***argv){

	
	int size;  // number of processes
 	int rank;  // current process id
  int squares_completed  = 0;
  int total_squares;

  	// MPI Initialization
	MPI_Init(argc, argv);
	MPI_Comm_rank(MPI_COMM_WORLD, & rank); // get id of current process
  MPI_Comm_size(MPI_COMM_WORLD, & size); // get number of processes
  	
  	// Set the ids on the board
  	initBoard(); 

  	double start_time; 
  	if(rank == 0)start_time = CycleTimer::currentSeconds();
	// Make each processor generate it's rectangles, and the IDs it is
  	// responsible for
  	for(int i = 0; i < pp; i+=size){
  		int index = i + rank;
  		if(index < pp){
			rects.push_back(getRects(xval[index],yval[index],pval[index]));
			ids.push_back(allids[index]);
			filled.push_back(0);
		}
	}
	// if(rank == _PROC){
	// cout<<"size: "<<size<<endl;
	// cout<<"rank: "<<rank<<endl;
	// cout<<"printing ids"<<endl;
	// for(int i = 0; i < rects.size(); i++){
	// 	cout<<ids[i]<<endl;	
	// }
	// }

	total_squares = ids.size();
	
	//if(rank == _PROC)printBoard();
	while(isAll() == 0){
		reset_cellboard();

		// Get the valid rectangles, for each ID
		for(int i = 0; i < total_squares; i ++){
			if(filled[i] != 1){
				getValid(ids[i],&(rects[i]));
			}
		}	
		
		// If one ID has only one rectangle, fill it up rightaway
		for(int i = 0; i < total_squares; i++){
			if(rects[i].size() == 1 && filled[i] != 1){
				fillUp(ids[i], rects[i].front()); // Fill the positions of a rectangle, with an id
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
		//if(rank == _PROC)printBoard();
		// Remove invalid rectangles, for each ID
		/*for(int i = 0; i < total_squares; i ++){
			if(filled[i] != 1){
				getValid(ids[i],&(rects[i]));
			}	
		}*/

		// Check board positions covered by only one rectangle
//		set<point> only = getOnly();

		// ... and fill the board with such rectangles
	/*	for(int i = 0; i < total_squares; i++){
			if(filled[i] != 1){
				onlyFill(i, only);

			}
		}*/

		// UPDATE BOARD IN ALL PROCESSES 
		//sync_board(rank, size);
	//	if(rank == _PROC)printBoard();
	}
	//cout<<"proc ended"<<endl;
  	MPI_Finalize();
  	if(rank == 0){
    	double end_time = CycleTimer::currentSeconds();
    	printf("time: %.16f sec\n", end_time - start_time);
  	}
}

int main(int argc, char **argv){

   
	
	solve_mpi(&argc, &argv);





}
