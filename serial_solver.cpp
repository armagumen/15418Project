#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstddef>
#include <iostream>
#include <omp.h>
#include <vector>
#include <set>

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

static int board[6][6] = {{0}};
static int pval[] = {4,2,9,10,5,6};
static int xval[] = {0,2,1,4,3,5};
static int yval[] = {0,0,2,3,5,5};
static int width = 6;
static int height = 6;
static int pp = 6;
static vector<rectangle> rects[6];
static int filled[6] = {0};
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

int main(){
	initBoard();
	for (int i = 0; i < pp; i++){
		getRects(xval[i],yval[i],pval[i],i);
	}

	for (int i = 0; i < pp; i++){
		std::cout<<i<<std::endl;
		getValid(i+1);
	}
	while(filnum < pp){
		for(int i = 0; i < pp; i++){
			if(rects[i].size() == 1 && filled[i] != 1){
				fillUp(i+1);
				filled[i] = 1;
				filnum++;
			}
		}
		for(int i = 0; i < pp; i++){
			if(filled[i] != 1){
				mustFill(i+1);
			}
		}

		for(int i = 0; i < pp; i++){
			if(filled[i] != 1){
				getValid(i+1);
			}
		}

		printBoard();
		//printf("??");
		std::cout<<std::endl;

	}






}
