#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 64
#define DEBUG 1
typedef struct listNode{
    int pageNum;
    struct listNode *prev;
    struct listNode *next;
    int index;
}node;

void insertNode(node **head, node **tail, int pn, int i){

    node *n = (node *) malloc(sizeof(node));
    if(n == NULL) exit(1);
    n->pageNum = pn;
    n->next = NULL;
    n->prev = NULL;
    n->index = i;
    if(*head == NULL){
        *head = n;
        *tail = n;
    }else{
        
        n->prev = *tail;
        (*tail)->next = n;
        *tail = n;
    }
}

void insertNodeLRU(node **head, node **tail, int pn, int i){
    node *n = *head;//start at head
    node * temp = NULL;//used to transverse the list
    while(n != NULL){
        //if node found with the same virtual page number
        if(n->pageNum == pn){
            //if it is the tail (or start), then no need to change queue
            if(n == *tail){
                 return;
            }
            //If it is the head, then things to the right
            else if(n == *head){
                *head = n->next;
                (*head)->prev = NULL;
            }
            //If in the middle, then move things around so node is not in the list
            else{
                n->next->prev = n->prev;
                n->prev->next = n->next;
            }//found node, so no more search
            break;
        }
        //iterate through list
        temp = n->next;
        n = temp; 
    }
    //if empty queue or node with page number not present in queue, then add to tail of queue
    if(n == NULL){
        insertNode(head, tail, pn, i);
    }else{
        //put the node found from the queue at the end
        //index and pagNum should be the same, but update next and prev, and the tail
        n->next = NULL;
        (*tail)->next = n;
        n->prev = *tail;
        *tail = n;
    }

}

node * getFirstNode(node**head, node**tail){
    node* n = NULL;
    if(*head != NULL){
        n = (*head);
        *head = n->next;
        (*head)->prev = NULL;
    }else{
        *tail = NULL;
    }
    return n;
}

void freeQueue(node **head){
    node *n = *head;
    node * temp = NULL;
    while(n != NULL){
        temp = n->next;
        free(n);
        n = temp; 
    }
}

int inArray(int a[], int i, int n){
    for(int j = 0; j < n; j++){
        if(a[j] == i) return j; 
        if(a[j] == -1) return -1;//if empty space, then it is not in there
    }
    return -1; 
}

int insertIntoArray(int a[], int i, int n){
    for(int j = 0; j < n; j++){
        if(a[j] == -1){
            a[j] = i; 
            return j;
        } 
    }return -1;
}

int replaceItemArray(int a[],int r, int i, int n){
    
    for(int j = 0; j < n; j++){
        if(a[j] == r){
            a[j] = i; 
            return j;
        } 
    }return -1; 
}

void printFrames(int a[], int n){
    
    for(int i = 0; i < n; i++){
        printf("#Frame %d: %d\n", i+1, a[i]);
    }
}

void pageReplace(FILE* fp, int frames, int algType){
    int framesUsed = 0;
    int pageFrames[frames];
    for(int i = 0; i < frames; i++){
        pageFrames[i] = -1;
    } 
    char *str = malloc(BUF_SIZE * sizeof(char));
    char *ptr;
    char const delim = ' ';
    int pageNum, pagePlace;
    char *action,*tok;
    node *head=NULL, *tail=NULL;
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
            pagePlace = inArray(pageFrames, pageNum, frames);
            if(pagePlace != -1){
                if(DEBUG == 1) printf("#%d is a hit!\n", pageNum);
                if(algType == 2) insertNodeLRU(&head, &tail, pageNum, pagePlace);
            }else{//If not, then page fault
                if(framesUsed < frames){//If there are room in page frame, insert, but page fault
                    int index = replaceItemArray(pageFrames, -1, pageNum, frames);
                    
                    if(algType == 1) insertNode(&head, &tail, pageNum, index);
                    if(algType == 2) insertNodeLRU(&head, &tail, pageNum, index); 
                    framesUsed ++;
                    //there might be something done different between reads/writes, so subject to change
                    if(*action == 'R'){
                        if(DEBUG == 1) printf("#Page fault, read issue\n");
                    }
                    if(*action == 'W'){
                        
                        if(DEBUG == 1) printf("#Page fault, write issue\n");
                    }
                }
                //No room in the frames, so replace one of the frames, which is where the algorithm
                //type comes into play
                else{
                    node *page = NULL;
                    //Need to replace replacing item to algorithm
                    if(algType == 1 || algType == 2){
                        page = getFirstNode(&head, &tail);
                    }
                    if(DEBUG == 1) printf("#Page Fault! Replacing %d with %d\n", page->pageNum, pageNum);
                    //replaceItemArray(pageFrames, replacePage, pageNum, frames);
                    pageFrames[page->index] = pageNum;
                    insertNode(&head, &tail, pageNum, page->index);
                    free(page);
                }
            }
        }
    }
    if(DEBUG == 1)  printFrames(pageFrames, frames);
    freeQueue(&head);
    free(str);
}

int main(int argc, char const *argv[]) {
    if(argc != 4){
        printf("Not enough arguements\n");
        exit(1);
    }
    
    int frames = atoi(argv[2]); 
    int curAlgo = -1; 
    if (strncmp("FIFO", argv[1], 4) == 0) curAlgo = 1;	
    if (strncmp("LRU", argv[1], 3) == 0) curAlgo = 2;
    FILE *fp;
    fp = fopen(argv[3], "r");
    if(fp){
        pageReplace(fp, frames, curAlgo);
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

