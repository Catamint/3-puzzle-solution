/*
*
*
*/

#include<stdio.h>
#include<stdlib.h> 
#include<time.h>
#define TQS 9  //total quantity of squares
#define FLOOR 30  //层数（步数）上限

typedef struct ListNode {
    int* square; //宫格
    int zero; //0的位置
    int* path; // == max steps
    struct ListNode* next; //下一节点
    struct ListNode* last; //上一节点
}node;

int step=0; //全局层数
int visitList[362881] = { 0 }; //if visited turn 0 to 1

node* newList(int* firstSquare);
node* newNode(int* square, int* path, int zero);
void LinkNodeIfront(node* parentNode, node* thisNode);
void LinkNodeIback(node* parentNode, node* thisNode);
void deleteNode(node* thisNode);

int zeroOf(int* square);
int completed(int* square);
int indexOf(int square[]);
int visited(int square[]);
int* neighbourOf(int zero);
int* blankSquare();
int* newSquare(int* parentSquare);
int* swap(int* square, int zero, int neighbour);
int* blankPath();
int* newPath(int* parentPath);
int* add(int* path, int zero);
node* printNode(node* node);

node* bfs(node* headNode);

void testneigh(int zero);

//以下为数据结构
//Create a new list and get the initial data (by arg).
node* newList(int* firstSquare) {      
    node* head = newNode(NULL,NULL,-1); //flag
    node* firstNode = newNode(newSquare(firstSquare) ,blankPath(), zeroOf(firstSquare));
    *(firstNode->path)=firstNode->zero;
    LinkNodeIfront(head,firstNode);
    return head;
} 
//new a node
node* newNode(int* square, int* path, int zero){
    node* thisNode = (node*)malloc(sizeof(node));
    if (thisNode == NULL) {
        printf("malloc failed.\n");
        return NULL;
    } 
    thisNode->last=NULL;
    thisNode->next=NULL;
    thisNode->square=square;
    thisNode->path=path;
    thisNode->zero=zero;
    return thisNode;
}
//link a node in front of parent
void LinkNodeIfront(node* parentNode, node* thisNode){
    if(parentNode->next!=NULL) parentNode->next->last=thisNode;
    thisNode->last = parentNode;
    thisNode->next = parentNode->next;
    parentNode->next = thisNode; 
}
//link a node in the back of parent
void LinkNodeIback(node* parentNode, node* thisNode){
    if(parentNode->last!=NULL) parentNode->last->next=thisNode;
    thisNode->last = parentNode->last;
    thisNode->next = parentNode;
    parentNode->last = thisNode; 
}
//delete the node
void deleteNode(node* thisNode) {
    if(thisNode!=NULL){
        node* nextNode = thisNode->next; //Q:可否以A->B->C访问结构体成员的成员？A:可.
        node* lastNode = thisNode->last;
        lastNode->next = nextNode;
        if(nextNode!=NULL) nextNode->last = lastNode;
        free(thisNode->square);
        free(thisNode->path);
        free(thisNode);        
    }
}

//以下为流程
//no answer 0 else 1, only when zero is in square[TQS]
int haveAnswer(int square[]){
    int inverseNumber=0; //逆序数
    int square1[TQS];
    int index_0=0; //0的位置
    for (int i = 0; i < TQS; i++)
    {
        if(square[i]==0) {
            square1[i]=TQS;
            index_0=i;
        }else{
            square1[i]=square[i];
        }
    }
    for(int i=0;i<TQS;i++){
        for(int j=0;j<i;j++){
            if(square1[j]>square1[i]) inverseNumber++;
        }
    }
    if((inverseNumber+index_0)%2==0) return 1;
    else return 0;
}
//only in start
int zeroOf(int* square){
    for(int i=0;i<TQS;i++){
        if (*(square+i) == 0) return i;
    }
    return -1;
}
//if completed return 1 else 0
int completed(int square[]){
    for(int i=0;i<TQS-1;i++){
        if (square[i] != i+1) return 0;
    }
    return 1;
}
//make an index for each square
int indexOf(int square[]){
    int number/*顺序数*/, jiecheng/*阶乘*/;
	int result = 0;
	for (int i = 0; i < 8; i++){
        number = 0;
		jiecheng = 1;
		if(8 - i != 0){
		    for(int j = 1; j <= 8 - i; j++){
			    jiecheng *= j;
			}
		}
		for(int j = 0; j <= i; j++){
			if (square[j] < square[i]) number += 1;
		}
		result += jiecheng * (square[i] - number); //获取当前坐标
	}
    return result;
}
//if visited return 1 else 0 && mark
int visited(int square[]){
	int indexOfSquare = indexOf(square); 
	if (visitList[indexOfSquare] == 1) return 1;
	else{
		visitList[indexOfSquare] = 1;
		return 0;
	}
}
//look for the neighbors and return a matrix(目前仅3阶)(warning:手动释放空间!!)
int* neighbourOf(int zero){
    int newZero;
    int* neighbour = (int*)malloc(sizeof(int)*4);
    for(int i=0;i<4;i++){
        neighbour[i]=-1;
    }
    for(int i=0;i<4;i++){
        newZero = zero+2*i-3;
        if(newZero<0||newZero>=9) continue;
        if(zero==3&&newZero==2) continue;
        if(zero==2&&newZero==3) continue;
        if(zero==6&&newZero==5) continue;
        if(zero==5&&newZero==6) continue;
        neighbour[i]=newZero;
    }
    return neighbour;
}
//create a blank square (have freed)
int* blankSquare(){
    int* thisSquare = (int*)malloc(sizeof(int)*TQS);
    if (thisSquare == NULL) {
        printf("malloc failed.\n");
        return NULL;
    } 
    return thisSquare;
} 
//create a new Square from parent (have freed)
int* newSquare(int* parentSquare){
    int* thisSquare = blankSquare();
    for (int i = 0; i < TQS; i++){
        *(thisSquare+i)=*(parentSquare+i);
    }
    return thisSquare;
}
//return the square after swapping(对换)
int* swap(int* square, int zero, int neighbour){
    int temp=*(square+zero);
    *(square+zero)=*(square+neighbour);
    *(square+neighbour)=temp;
    return square;
}
//create a blank path (have freed)
int* blankPath(){
    int* thisPath = (int*)malloc(sizeof(int)*FLOOR);
    if (thisPath == NULL) {
        printf("malloc failed.\n");
        return NULL;
    } 
    return thisPath;
} 
//create a new path from parent (have freed)
int* newPath(int* parentPath){
    int* thisPath = blankPath();
    for (int i = 0; i < FLOOR; i++)
    {
        *(thisPath+i)=*(parentPath+i);
    }
    return thisPath;
}
//return the path after add
int* add(int* path, int zero){
    *(path+step)=zero;
    return path;
}
//printNode
node* printNode(node* node) {
    printf("square=");
    for(int i=0;i<TQS;i++){
        printf("%d ",node->square[i]);
    }
    printf("\nstep=");
    for(int i=0;i<=step;i++){
        printf("%d ",node->path[i]);
    }
    printf("\n");
    printf("zero=%d,step=%d\n",node->zero, step+1);
    printf("\n");

    return node;
}
//
void printList(node* head){
    node* temp = head;
    printf("-------list=\n");
    while(temp->next!=NULL){
        temp=printNode(temp->next);
    }
    printf("-------end\n");
}
//bfs
node* bfs(node* headNode){
    node*thisNode=headNode->next;
    int* neighbour=NULL;
    int* thisNeighbour=NULL; 
    node* tempNode=NULL;
    int* tempSquare=NULL;
    int* tempPath=NULL;

    do{
        do{
            //printf("\n\nnextfloor\n");////
            neighbour=neighbourOf(thisNode->zero);
            for(int i=0;i<4;i++){
                thisNeighbour=neighbour+i;
                if (*(thisNeighbour) == -1) continue; //if no neighbour in this direction
                //printf("nextnode\n");////
                tempSquare=swap(newSquare(thisNode->square),thisNode->zero,*thisNeighbour);
                if(visited(tempSquare)){
                    free(tempSquare); 
                    continue;
                }
                tempPath=newPath(add(thisNode->path,*thisNeighbour));
                tempNode=newNode(tempSquare,tempPath,*thisNeighbour);
                //printNode(tempNode);////
                if(completed(tempSquare)) return tempNode; 
                else LinkNodeIback(thisNode,tempNode); //
                //printList(headNode);////
            } //一个node
            free(neighbour);
            tempNode=thisNode;
            if(thisNode->next!=NULL){
                thisNode=thisNode->next;
                deleteNode(tempNode);
            }else{
                free(headNode->square); 
                free(headNode->path);  
                headNode->square=newSquare(tempNode->last->square);
                headNode->path=newPath(tempNode->last->path);
                headNode->zero=tempNode->last->zero;
                thisNode=headNode->next;
                //printList(headNode);////
                deleteNode(tempNode);
                break;
            }
        }while (1);
    }while(step++ <= FLOOR); //超层
    return NULL;
}

void testneigh(int zero){
    for(int j=0;j<9;j++){
        int* nei=neighbourOf(j);
        printf("\n%d: ", j+1);
        for(int i=0;i<4;i++){
            printf("%d ",(*nei)+1);
            nei++;
        }
        free(nei);
    }   
}

int main() {
    int square[TQS]={0,6,8,7,1,5,3,2,4};   /* 当前最多步数24 0,6,8,7,1,5,3,2,4 */
    node* answer = NULL;
    /*int square[TQS];
    for (int i = 0; i < TQS; i++){
        scanf("%d",&square[i]);
    }*/
    if(completed(square)){
        printf("Have completed....."); //首次完成检查
    }else if(!haveAnswer(square)){
        printf("Absolutely have no answer!"); //有解检查
    }else{
        answer = bfs(newList(square));
        if(answer!=NULL) {
            printf("completed!\n");
            printNode(answer);
        }else{
            printf("No anwswer in %d steps.", FLOOR);
        }
        //printNode(BFSList->next);
        //testneigh(3);
    }
    printf("time=%.2fs\n",(double)clock()/CLOCKS_PER_SEC);
    getchar();
}


/*

node* search_byLocation(node* head, int loc) {
    node* temp = head;
    for (int i = 0; i < loc; i++)
    {
        if (temp->next == NULL) {
            printf("Overflow!!");
            return NULL;
        }
        temp = temp->next;
    }
    return temp;
}

*/