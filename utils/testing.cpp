#include "testing.h"

shikaku_input parse_input_file(char* filename){
	ifstream input_file;
	string line;
	shikaku_input result;

	input_file.open(filename);
	
	// Set width and height of the board
	getline(input_file, line);
	sscanf(line.c_str(), "%d,%d", &(result.bwidth), &(result.bheight));
	
	// Add the requirements to the input struct
	while(getline(input_file, line)){
		shikaku_req tmp;
		sscanf(line.c_str(), "%d,%d,%d", &(tmp.x), &(tmp.y), &(tmp.value));
		result.requirements.push_back(tmp);
	}
	
	input_file.close();
	return result;
}

int test_correctness(int **board, int b_size, map<int, int> result){
	int **visited = (int **) malloc(sizeof(int *) * b_size);
	for(int i = 0; i < b_size; i++){
		visited[i] = (int *) malloc(sizeof(int) * b_size);
		for(int j = 0; j < b_size; j++){
			visited[i][j] = 0;
		}
	}
	
	// Check correctness 

	for(int i = 0; i < b_size; i++){
		for(int j = 0; j < b_size; j++){
			if(!visited[i][j]){
				int count = get_contiguous_count(board, visited, i, j, b_size);
				if(result.count(board[i][j]) != 1 || count != result[board[i][j]]){
					return 0;
				}
			}
		}
	}


	for(int i = 0; i < b_size; i++){
    free(visited[i]);
  }
  free(visited);	
	return 1;
}

int get_contiguous_count(int **board, int** status, int i, int j, int b_size){
	int count = 0;
	int current_id = board[i][j];
	if(status[i][j] == 0){
		count++;
		status[i][j] = 1;
	}
	// Check up/down/left/rigth recursively
	if(i > 0 && status[i-1][j] == 0 && board[i-1][j] == current_id){
		count += get_contiguous_count(board, status, i - 1, j, b_size);
	}
	if(j > 0 && status[i][j-1] == 0 && board[i][j-1] == current_id){
		count += get_contiguous_count(board, status, i, j - 1, b_size);
	}
	if(i < b_size - 1 && status[i+1][j] == 0 && board[i+1][j] == current_id){
  	count += get_contiguous_count(board, status, i + 1, j, b_size);
	}
	if(j < b_size - 1 && status[i][j+1] == 0 && board[i][j+1] == current_id){
		count += get_contiguous_count(board, status, i, j + 1, b_size);
	}

	return count;
}

// Helper function to print the board, represented as an matrix of ints
void print_board(int **board, int b_size){
	cout<<"---- Board ----"<<endl;
	for(int i = 0; i < b_size; i++){
		for(int j = 0; j < b_size; j++){
			cout<<board[i][j]<<"\t";
		}
		cout<<endl;
	}
	cout<<"---- End of Board ----"<<endl;
}
void print_results(map<int,int> results){
	cout<<"---- Results id:count ----"<<endl;
	for(map<int, int>::iterator it = results.begin(); it != results.end(); it++){
		cout<<it->first<<": "<<it->second<<endl;
	}
	cout<<"---- End of results ----"<<endl;
}
void print_input(shikaku_input in){
	cout<<"--- Input ---"<<endl;
	cout<<"Board width: "<<in.bwidth<<endl;
	cout<<"Board height: "<<in.bheight<<endl;
	cout<<"Requirements:"<<endl;
	for(int i = 0; i < in.requirements.size(); i++){
		cout<<"x: "<<in.requirements[i].x<<"\ty: "<<in.requirements[i].y<<"\tvalue: "<<in.requirements[i].value<<endl;
	}

}
void deallocate_shikaku_result(shikaku_result* result){
	// Deallocate board memory
	for(int i = 0; i < result->bwidth; i++){
		free(result->board[i]);
	}
	free(result->board);
	free(result);

}
