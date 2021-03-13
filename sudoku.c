/*
	How code run:
		the code will create 27 threads(for 9 rows, 9 cols,9 box(3X3).),
		for each thread will be passed in a parameter which contain begin row and col index, 
		and the id repeasent this row/col/box.
		Also each thread will be passed in a checking funtion to check the row/col/box is vaild or not
		whatever the row/col/box passed into the check funtion => the function will out put
		a message about the result.
		The final result will save into a file.
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define K 9 //pre define the max size at 9 for each row/col/box

//the sudoku data
int sudoku[K][K] = {
{6,2,4,5,3,9,1,8,7},
{5,1,9,7,2,8,6,3,4},
{8,3,7,6,1,4,2,9,5},
{1,4,3,8,6,5,7,2,9},
{9,5,8,2,4,7,3,6,1},
{7,6,2,3,9,1,4,5,8},
{3,7,1,9,5,6,8,4,2},
{4,9,6,1,8,2,5,7,3},
{2,8,5,4,7,3,9,1,6}
};

//file var
FILE *fp;

//the unit strut for row/col/box
typedef struct {
	int row;//row index
	int col;//col index
	int indexAt;//which row/col/box it repesented
}parameters;

//function for checking a row 
void *checkRowEle(void *param){
	parameters *temp = (parameters*) param;
	int row = temp->row;
	int col = temp->col;
	int id = temp->indexAt;
	int checkList[K]={0,0,0,0,0,0,0,0,0}; //the array for counting if a number in row is repeated, 0=>not repeat, 1=> repeated
	for(int i=0;i<K;i++){ //going over all 9 element in the row
		int element = sudoku[row][i];//element repesent the current number in row
		if(element<=0 || element >9){//check if the element is in correct range
			fprintf(fp,"Row%d dose not staisfy the condition.\n",id);
			pthread_exit(NULL);
		}
		//check if the number is repeated
		if(checkList[element-1]==0){//not repeated
			checkList[element-1]=1;//set this element in the recored 
		}else{
			//the element is alreay show up once, exit the thread
			fprintf(fp,"Row%d dose not satisfy the condition.\n",id);
			pthread_exit(NULL);
		}
	}
	//this point mean the row pass the test, the row is vailed
	fprintf(fp,"Row%d satisfy the conditon.\n",id);
	pthread_exit(NULL);
}

//function for checking a col 
void *checkColEle(void *param){
	parameters *temp = (parameters*) param;
	int row =  temp->row;
	int col = temp->col;
	int id =temp->indexAt;
	int checkList[K] ={0,0,0,0,0,0,0,0,0};//the array for counting if a number in col is repeated, 0=>not repeat, 1=> repeated
	for(int i=0;i<K;i++){//going over all 9 element in the row
		int element = sudoku[i][col];
		if(element<=0 || element >9){//check if the element is in correct range
			fprintf(fp,"Column%d dose not satisfy the condition.\n",id);
			pthread_exit(NULL);
		}
		//check if the number is repeated
		if(checkList[element-1]==0){// meaning 0=>not repeat, 1=> repeated
			checkList[element-1]=1;
		}else{
			fprintf(fp,"Column%d dose not satisfy the condition.\n",id);
			pthread_exit(NULL);
		}
	}
	//this point mean the col pass the test, the col is vailed
	fprintf(fp,"Column%d satisfy the condition.\n",id);
	pthread_exit(NULL);
}

//function for checking a box 3x3  
void *checkBox(void *param){
	parameters *temp = (parameters*) param;
	int row = temp->row;
	int col= temp->col;
	int id = temp->indexAt;
	int checkList[K]={0,0,0,0,0,0,0,0,0};//the array for counting if a number in col is repeated, 0=>not repeat, 1=> repeated
	//going over each element in the box
	for(int i=row;i<row+3;i++){//starting at row, row+1, row+2
		for(int j=col;j<col+3;j++){//starting at col, col+1, col+2
			int element = sudoku[i][j];//current element in the box
			if(element<=0 || element>9){//check if in the correct range
				fprintf(fp,"Box%d does not statisfy the condition,\n",id);
				pthread_exit(NULL);
			}
			//check if the number show up repeatly
			if(checkList[element-1]==0){
				checkList[element-1]=1;
			}else{
				// the number been repeated, exit the thread
				fprintf(fp,"Box%d dose not satisfy the condition.\n",id);
				pthread_exit(NULL);
			}
		}
	}
	//this point mean the box pass the test, the box is vailed
	fprintf(fp,"Box%d satisfies the condition.\n",id);
	pthread_exit(NULL);
}

int main(int argc,char *argv[]){

	//sudoku[2][2] = 0;//this is intend to set equal 0 => for invaild sudoku
	fp = fopen("result_correct.txt","w");//creat file for saving result         //if test the for didffernt sudoku can change the file name

	pthread_t threads[27];// the total 27 threads will be created.
	int threadID=0;//the threadID started at 0

	fprintf(fp, "Here is the output:\n\n");

	sleep(1);
	//for the 9 row 
	for(int i=0;i<K;i++){
		parameters *paramR = (parameters *)malloc(sizeof(parameters));//give space for parpamteters
		paramR->row=i;
		paramR->col=0;
		paramR->indexAt= i;//passing i as each row index
		pthread_create(&threads[threadID],NULL,checkRowEle,paramR);//creating the thread 
		threadID++;//inceasing the thread id
	}
	sleep(1);//sleep for about 1 second , let the thread message going back by the order
	//for 9 columns
	for(int j=0;j<K;j++){
		parameters *paramC = (parameters*)malloc(sizeof(parameters));
		paramC->row=0;
		paramC->col=j;
		paramC->indexAt=j;
		pthread_create(&threads[threadID],NULL,checkColEle,paramC);
		threadID++;
	}
	sleep(1);//sleep for about 1 second , let the thread message going backa by the order
	//for 9 boxes
	for(int r=0;r<K;r+=3){
		for(int c=0;c<K;c+=3){
			char index[9];//special for box index which will be comine with row+col
			snprintf(index,9,"%d%d",r,c);//turn the row+col into string
			parameters *paramB = (parameters*)malloc(sizeof(parameters));
			paramB->row=r;
			paramB->col=c;
			paramB->indexAt=atoi(index);//changeback to int and pass as index repesent start area of box
			pthread_create(&threads[threadID],NULL,checkBox,paramB);
			threadID++;
		}
	}
	sleep(1);//sleep for about 1 second , let the thread message going back by the order
	//join all information
	for(int z=0;z<27;z++){
		pthread_join(threads[z],NULL);
	}
	fclose(fp);
	return 0;
}
