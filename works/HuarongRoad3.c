/*
*
*
*/

#include<stdio.h>
#include<stdlib.h> 
#define TQS 9  //total quantity of squares
#define FLOOR 20  //层数（步数）上限

typedef struct ListNode {
    int* square; //宫格
    int zero; //0的位置
    int* path; // == max steps
    struct Listnode* last; //上一节点
    struct ListNode* next; //下一节点
}node;

int step=0; //全局层数

int zeroOf(int square[]);
node *newList(int square[]);
node *blankNode();
void LinkNodeIfront(node *parentNode, node *thisNode);
void linkNodeIback(node *parentNode, node *thisNode);
void deleteNodeIfront(node *parentNode);
void deleteNodeIback(node* parentNode);
void inputSquare(int square[], node *thisNode);
int completed(int square[]);
int visited();
int* neighbourOf(int zero);
void addNode_old(node* parentNode, int square[], int zero);
void bfs(node* headNode);

//以下为数据结构
//Create a new list and get the initial data (by arg).
node* newList(int square[]) {      
    node* head = blankNode(); //DO NOT SAVE DATA to head node!! 
    node* firstNode = blankNode();
    head->last=NULL;
    head->zero=-1; //flag
    inputSquare(square, firstNode);
    firstNode->zero=zeroOf(square);
    LinkNodeIfront(head,firstNode);
    return head;
} 
//create a empty node
node* blankNode(){
    node* thisNode = (node*)malloc(sizeof(node));
    if (thisNode == NULL) {
        printf("malloc failed.\n");
        return NULL;
    } 
    thisNode->last=NULL;
    thisNode->next=NULL;
    return thisNode;
}
//link a node in front of parent
void LinkNodeIfront(node* parentNode, node* thisNode){
    thisNode->last = parentNode;
    thisNode->next = parentNode->next;
    parentNode->next = thisNode; 
}
//link a node in the back of parent
void linkNodeIback(node* parentNode, node* thisNode){
    thisNode->last = parentNode->last;
    thisNode->next = parentNode;
    parentNode->last = thisNode; 
}
//delete the node in front of parent
void deleteNodeIfront(node* parentNode) {
    node* thisNode = parentNode->next;
    parentNode->next = thisNode->next;
    parentNode->next->last = parentNode; //为什么这里可以??
    free(thisNode);
}
//delete the node in front of parent
void deleteNodeIback(node* parentNode) {
    node* temp=NULL;
    node* thisNode = parentNode->last;
    parentNode->last = thisNode->last;
    temp = parentNode->last; //貌似不能访问结构体成员的成员, 故引入temp, 待解.
    temp->next = parentNode;
    free(thisNode);
}

//以下为流程
//only in start
int zeroOf(int square[]){
    for(int i=0;i<TQS;i++){
        if (square[i]==0) return i;
    }
    return -1;
}
//if completed return 1 else 0
int completed(int square[]){
    for(int i=1;i<TQS;i++){
        if (square[i]!=i) return 0;
    }
    printf("completed!");
    return 1;
}
//
int visited();
//look for the neighbors and return a matrix(目前仅3阶)(warning:手动释放空间!!)
int* neighbourOf(int zero){
    int newZero;
    int* neighbour = malloc(sizeof(int)*4);
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

//input the square to a new matrix
void inputSquare(int square[],node* thisNode){
    for(int i=0;i<TQS;i++){
        thisNode->square[i]=square[i];
    }
} 
//input the path to a new matrix
void inputPath(int path[],node* thisNode){
    for(int i=0;i<FLOOR;i++){
        thisNode->path[i]=path[i];
    }
} 

//对换
void swap(int square[], int zero, int neighbour){
    int temp=square[zero];
    square[zero]=square[neighbour];
    square[neighbour]=temp;
}


void bfs(node* headNode){
    node*thisNode=headNode->next;
    int* neighbour=NULL; 
    int tempsquare[TQS];
    do{
        do{
            neighbour=neighbourOf(thisNode->zero);
            for(int i=0;i<4;i++,neighbour++){
                if (*neighbour == -1) continue; // Make sure itsnot the Head
                
                tempNode=blankNode();
                /*1*/if(completed(thisNode->square)) return; 
            //  /*2*/if:visited
                
                inputSquare();
                linkNodeIback();
                //

            } //一个node
            free(neighbour);
            thisNode=thisNode->next;
            //deletenode

        }while (thisNode=NULL); //一轮
        step++;
        thisNode=headNode->next;
    }while(step>=FLOOR); //超层
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
    //int square[TQS]={1,2,3,4,5,6,7,8,0};
    //node* BFSList = newList(square);
    testneigh(3);
    getchar();
}


/*
void printList(node* head) {
    node* temp = head;
    while (temp->next != NULL)
    {
        temp = temp->next;
        for(int i=0;i<TQS;i++){
            printf("%d",temp->square[i]);        
        }
        printf("\n");
    }
}
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