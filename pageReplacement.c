#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 64

int inArray(int a[], int i, int n){
    for(int j = 0; j < n; j++){
        if(a[j] == i) return 1; 
        if(a[j] == -1) return 0;//if empty space, then it is not in there
    }
    return 0; 
}

void insertIntoArray(int a[], int i, int n){

    for(int j = 0; j < n; j++){
        if(a[j] == -1){
            a[j] = i; 
            return;
        } 
    }
}

void replaceItemArray(int a[],int r, int i, int n){
    
    for(int j = 0; j < n; j++){
        if(a[j] == r){
            a[j] = i; 
            return;
        } 
    }
    
}

void pageReplace(FILE* fp, int frames){
    int framesUsed = 0;
    int pageFrames[frames];
    for(int i = 0; i < frames; i++){
        pageFrames[i] = -1;
    } 
    char *str = malloc(BUF_SIZE * sizeof(char));
    char *ptr;
    char const delim = ' ';
    int pageNum;
    char *action,*tok;

    while(!feof(fp)){
        ptr = fgets(str, BUF_SIZE, fp);
        //read line, and if we can
        if(ptr){
            //get the action, either read (R) or write(W)
            action = strtok(str, &delim);
            //Get the virtual page number
            tok = strtok(NULL, &delim);
            pageNum = atoi(tok);
            //If the virtual page number is in pageframes, then it is a hit
            if(inArray(pageFrames, pageNum, frames)){
                printf("It's a hit!\n");
            }else{//If not, then page fault
                if(framesUsed < frames){//If there are room in page frame, insert, but page fault
                    replaceItemArray(pageFrames, -1, pageNum, frames);
                    framesUsed ++;
                    //there might be something done different between reads/writes, so subject to change
                    if(*action == 'R'){
                        printf("read issue\n");
                    }
                    if(*action == 'W'){
                        
                        printf("write issue\n");
                    }
                }
                //No room in the frames, so replace one of the frames, which is where the algorithm
                //type comes into play
                else{
                    //Need to replace replacing item to algorithm
                    printf("Replacing %d with %d\n", pageFrames[0], pageNum);
                    replaceItemArray(pageFrames, pageFrames[0], pageNum, frames);
                }
            }
        }
    }
    free(str);
}

int main(int argc, char const *argv[]) {
    if(argc != 4){
        printf("Not enough arguements\n");
        exit(1);
    }
    
    int frames = atoi(argv[2]); 
    FILE *fp;
    fp = fopen(argv[3], "r");
    if(fp){
        pageReplace(fp, frames);
        if(fclose(fp) != 0){
            printf("cannot close file %s\n", argv[3]);
            exit(1);
        }
	}
    else{//In case we can't open a file
        printf("Cannot open file %s\n", argv[3]);
        exit(1);
    }
	
    return 0;
}

