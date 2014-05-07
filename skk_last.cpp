#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstddef>
#include <iostream>
#include <omp.h>
#include <vector>
#include <set>
#include "CycleTimer.h"

using std::vector;
using std::set;
using std::string;
//#include "CycleTimer.h"

typedef struct{
	int x1;
	int y1;
	int x2;
	int y2;
} rectangle;

typedef struct{
	int x;
	int y;

}point;

bool operator<(const point& first, const point& second){
	return (first.x + 1000 * first.y) < (second.x + 1000 * second.y);
}
#define _BWIDTH 6
#define _BHEIGHT 6
#define _PVAL_SIZE (sizeof(pval)/sizeof(int))
static int board[_BWIDTH][_BHEIGHT] = {{0}};
static int cellboard[_BWIDTH][_BHEIGHT] = {{0}};
static int pval[] = {4,2,9,10,5,6};
static int xval[] = {0,2,1,4,3,5};
static int yval[] = {0,0,2,3,5,5};

//static int pval[] = {4,9,6,4,6,9,6,4,6,4,4,2,4,4,8,3,8,4,6,4,3,9,6,4,3,4,6,6,6,4,4,4,2,6,2,6};
//static int xval[] = {0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,4,4,4,5,5,5,6,6,6,7,7,7,7,8,8,8,8,9,9,9,9};
//static int yval[] = {0,6,8,16,7,9,14,17,1,3,12,15,2,6,13,1,7,9,8,10,16,4,11,15,2,5,14,16,0,3,8,10,1,9,11,17};
static int pp = _PVAL_SIZE;
static int width = _BWIDTH;
static int height = _BHEIGHT;
static vector<rectangle> rects[_PVAL_SIZE];
static int filled[_PVAL_SIZE] = {0};
static int filnum = 0;


void initBoard(){
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

void getRects(int x, int y, int p,int id){
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
						// ARmando added id here, instead of i
						rects[id].push_back(newrect);
					}
				}
			}
		}
	}

}

void getValid(int num){

	vector<rectangle>::iterator itt = rects[num-1].begin();
	for(; itt != rects[num-1].end();itt++){
		int check = 1;
		for(int j = (*itt).y1; j < (*itt).y2+1;j++){
			for(int i = (*itt).x1; i < (*itt).x2+1; i++){

				if(board[j][i] != num && board[j][i] != 0){
					check = 0;
				}

			}
		}
		if(check == 0){
			itt = rects[num-1].erase(itt);
			itt--;
		}


	}

}

void fillUp(int num){
	rectangle r = rects[num-1].front();
	for (int j = r.y1; j < r.y2+1;j++ ){
		for (int i = r.x1; i < r.x2+1;i++){
			board[j][i] = num;
		}
	}

}

void fillSet(set<point> res,int num){
	std::set<point>::iterator it1 = res.begin();
	for(; it1 != res.end();it1++){
		board[(*it1).y][(*it1).x] = num;
	}
}

void mustFill(int num){
	vector<rectangle>::iterator itt = rects[num-1].begin();
	int start = 0;
	set<point> res;
	for(; itt != rects[num-1].end();itt++){
		start++;
		set<point> comp;
		comp.clear();
		for(int j = (*itt).y1; j < (*itt).y2+1;j++){
			for(int i = (*itt).x1; i < (*itt).x2+1; i++){
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
			std::set<point>::iterator it1 = res.begin();
			std::set<point>::iterator it2 = comp.begin();
			while ( (it1 != res.end()) && (it2 != comp.end()) ) {
			    if (*it1 < *it2) {
			    	res.erase(it1++);
			    } else if (*it2 < *it1) {
			    	++it2;
			    } else { // *it1 == *it2
			            ++it1;
			            ++it2;
			    }
			}
			res.erase(it1, res.end());
		}

	}
	fillSet(res,num);
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

void onlyFill(int num, set<point> o){
	vector<rectangle>::iterator itt = rects[num-1].begin();
	set<point> res;
		for(; itt != rects[num-1].end();itt++){
			set<point> comp;
			comp.clear();
			for(int j = (*itt).y1; j < (*itt).y2+1;j++){
				for(int i = (*itt).x1; i < (*itt).x2+1; i++){
					point pt; pt.x = i; pt.y = j;
					if(o.count(pt) == 1){
						fillUpInd(num,(*itt).x1,(*itt).x2,
									(*itt).y1, (*itt).y2);
						filled[num-1] = 1;
						filnum +=  1;
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

int main(){
	initBoard();
	#	pragma omp parallel for
	for (int i = 0; i < pp; i++){
		getRects(xval[i],yval[i],pval[i],i);
	}

	#	pragma omp parallel for  
	for (int i = 0; i < pp; i++){
		std::cout<<i<<std::endl;
		getValid(i+1);
	}
	std::cout<<"pre while"<<std::endl;
	while(filnum < pp){
		#	pragma omp parallel for  
		for(int i = 0; i < pp; i++){
			if(rects[i].size() == 1 && filled[i] != 1){
				fillUp(i+1);
				filled[i] = 1;
				filnum++;
			}
		}

		#	pragma omp parallel for  
		for(int i = 0; i < pp; i++){
			if(filled[i] != 1){
				mustFill(i+1);
			}
		}

		#	pragma omp parallel for  
		for(int i = 0; i < pp; i++){
			if(filled[i] != 1){
				getValid(i+1);
			}
		}

		set<point> only = getOnly();
		#	pragma omp parallel for  
		for(int i = 0; i < pp; i++){
			if(filled[i] != 1){
				onlyFill(i+1, only);

			}
		}

		printBoard();
		//printf("??");
		std::cout<<std::endl;
		/*std::cout<<"CELLBOARD----"<<std::endl;
		for(int i = 0; i < 6; i++){
			for(int j = 0; j < 6; j++){
				std::cout<<cellboard[i][j];
			}
			std::cout<<std::endl;
		}	*/
	  reset_cellboard();
	}






}
