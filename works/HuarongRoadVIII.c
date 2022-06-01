/*3-PUZZLE SOLVER VIII
*
*三阶数字华容道求解器，可以
*1 输入一个开局，然后得到一个最优解
*2 统计所有开局的最优解分布情况
*3 保存所有开局的最优解到文件
*4 从文件读取任意开局的最优解
* @Catamint
*/

#include<stdio.h>
#include<conio.h>
#include<stdlib.h> 
#include<time.h>
#include<string.h>
#include<math.h>

#define ORDER 3  //(阶数)
#define TQS 9    // total quantity of squares. (ORDER^2)
#define FLOOR 33 // maximum of steps.

typedef struct ListNode{
    int *square; // [FLOOR] (宫格) 4 B + (4*9 B)
    int zero;    // Location of blank square. 4 B
    int *path;   // [TQS] 4 B + (4*33 B)
    struct ListNode *next; //4 B
    struct ListNode *last; //4 B
    //188 B * 181440 == 34 MB in total.
} node;

struct answer{
    int square;
    int step;
    int* paths[10];
}answers[362881];

int step = 0;                //层数/步数.
int totalSquares = 0;       // amount of squares been visited.
int visitList[362881] = {0}; // if visited turn to 1.
int stepsList[FLOOR] = {0};  //存放步数为[]的数量.
int openingSquare[TQS];     //输入的square,即终点.
int rangeSquare[TQS] = {1, 2, 3, 4, 5, 6, 7, 8, 0};

//流程：
void inatialization();
int getMode();
void doMode_1();
void doMode_2();

void doMode_3();
int append_answer(int square, int id, int* path);
int print_array(int* array, int lenth, FILE* file);
int printAnswerList(FILE* file);

//链表结构：
node* newList();
node* newNode(int* square, int* path, int zero);
void LinkNodeIfront(node* parentNode, node* thisNode);
void linkNodeIback(node* parentNode, node* thisNode);
void deleteNode(node* thisNode);
//节点结构：
int* blankSquare();
int* newSquare(int* parentSquare);
int* swap(int* square, int zero, int neighbour);
int* blankPath();
int* newPath(int* parentPath);
int* add(int* path, int zero);
//数据操作：
node* bfsToFile(node* headNode, char mode);
int haveAnswer(int square[]);
int zeroOf(int* square);
int openingEqualsTo(int* square);
int indexOf(int square[]);
int visited(int square[]);
int* neighbourOf(int zero);

//输入输出：
int scanTheSquare3();
node* printNode(node* node);


//test:
int split(char* line,char list[][FLOOR+1]);
int doMode_4();
int arrayToInt(int* array,int lenth);

int doMode_4(){
    FILE* answers_text=NULL;
    char line[500];
    char answer[12][FLOOR+1];
    int amount=0;
    int index=indexOf(openingSquare);
    if((answers_text=fopen("answers.csv","r"))==NULL) {
        printf("faild to open the file.");
        return -1;
    }
    for(int current_line=0;current_line<index;current_line++){
        fgets(line,500,answers_text);
    }
    amount=split(line,answer)-3;
    if(openingEqualsTo(rangeSquare)){
    printf("Have completed.....\n"); 
    }else if(amount==0){
        printf("Absolutely have no answer!"); 
    }else{
        printf("Completed!\n");
        for (int i = 3; i < amount+3; i++){
            printf("%s\n",answer[i]);
        }
    }
    fclose(answers_text);
}
int split(char* line,char list[][FLOOR+1]){
    char* head=line;
    char* end=line-1;
    int amount_of_paths=0;
    while (*end++!='\n'){
        head=end;
        for(int i=0;i<FLOOR+1;i++,end++){
            if(*end==','||*end=='\n'){
                memcpy(list[amount_of_paths++],head,i); 
                break;
            }
        }
    }
    return amount_of_paths;
}

void help(){

}



int main() {
    printf("3-PUZZLE SOLVER v4\n");
    do{
        printf("\n");
        printf("[1]=solve a puzzle\t[2]=statistics all\n[3]=create file\t\t[4]=solve by file\n");
        printf("Press [?] for help; [q] to quit.\n");
        while(getMode()==-1){
            printf("\b\b\b   \b\b\b");
        };
        inatialization();
        //printf("time=%.2fs\n",(double)clock()/CLOCKS_PER_SEC);
    }while(1);
    return 0;
}


//数据初始化
void inatialization(){
    step=0; //全局层数.
    totalSquares=0; //全局组合数.
    for (int i = 0; i < 362881; i++){
        visitList[i] = 0;
    }
    for (int i = 0; i < FLOOR; i++){
        stepsList[i] = 0;
    }
    for (int i = 0; i < 362881; i++){
        for(int j=0;j<10;j++){
            answers[i].paths[j]=NULL; 
        }
    }
    
}

//宽度搜索3
node* bfsToFile(node* headNode, char mode){
    node*thisNode=headNode->next;
    int* neighbour=NULL;
    node* tempNode=NULL;
    int* tempSquare=NULL;
    do{
        if (thisNode==headNode){
            if(thisNode->next==thisNode) return NULL;
            thisNode=thisNode->next;
            step++;
            continue;
        }
        neighbour=neighbourOf(thisNode->zero);
        do{
            tempSquare=swap(newSquare(thisNode->square),thisNode->zero,*neighbour);
            if(visited(tempSquare)){
                if(mode=='3' && step==answers[indexOf(tempSquare)].step){
                    append_answer(arrayToInt(tempSquare,TQS),indexOf(tempSquare),newPath(add(thisNode->path,*neighbour)));
                }
                free(tempSquare);
                continue;
            }
            tempNode=newNode(tempSquare, newPath(add(thisNode->path,*neighbour)), *neighbour);
            linkNodeIback(thisNode,tempNode); //
            if(mode=='1' && openingEqualsTo(tempSquare)) return tempNode; //return the answer.
            else if(mode=='2'){
                totalSquares++;
                stepsList[step]++;
            }
            else if(mode=='3'){
                append_answer(arrayToInt(tempSquare,TQS),indexOf(tempSquare),tempNode->path);
            }
        }while(*(++neighbour) != -1); //
        thisNode=thisNode->next;
        deleteNode(thisNode->last);
    }while (step<FLOOR);
    return NULL;
}


//以下为数据结构
//Create a new list and get the initial data (by arg).
node* newList() {      
    node* head = newNode(NULL,NULL,-1);
    node* firstNode = newNode(rangeSquare, blankPath(), TQS-1);
    *(firstNode->path)=firstNode->zero;
    head->last=firstNode;
    head->next=firstNode;
    firstNode->last=head;
    firstNode->next=head;//实现循环
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
//link a node in front of parent.
void LinkNodeIfront(node* parentNode, node* thisNode){
    if(parentNode->next!=NULL) parentNode->next->last=thisNode;
    thisNode->last = parentNode;
    thisNode->next = parentNode->next;
    parentNode->next = thisNode; 
}
//link a node in the back of parent.
void linkNodeIback(node* parentNode, node* thisNode){
    if(parentNode->last!=NULL) parentNode->last->next=thisNode;
    thisNode->last = parentNode->last;
    thisNode->next = parentNode;
    parentNode->last = thisNode; 
}
//delete the node.
void deleteNode(node* thisNode) {
    if(thisNode!=NULL){
        node* nextNode = thisNode->next;
        node* lastNode = thisNode->last;
        lastNode->next = nextNode;
        if(nextNode!=NULL) nextNode->last = lastNode;
        free(thisNode->square);
        //free(thisNode->path); //在最后一个square意外暂停？(已解决，但不知何故这一行效率极低)(mode3需要paths,不改了)
        free(thisNode);
        thisNode=NULL;
    }
}

//以下为node内部结构及操作.
//create a blank square.
int* blankSquare(){
    int* thisSquare = (int*)malloc(sizeof(int)*TQS);
    if (thisSquare == NULL) {
        printf("malloc failed.\n");
        return NULL;
    } 
    return thisSquare;
} 
//create a new Square from parent.（未来会改用指针数组）
int* newSquare(int* parentSquare){
    int* thisSquare = blankSquare();
    memcpy(thisSquare,parentSquare,sizeof(int)*TQS);
    return thisSquare;
}
//return the square after swapping(对换).
int* swap(int* square, int zero, int neighbour){
    int temp=*(square+zero);
    *(square+zero)=*(square+neighbour);
    *(square+neighbour)=temp;
    return square;
}
//create a blank path. (have fred)
int* blankPath(){
    int* thisPath = (int*)malloc(sizeof(int)*(FLOOR+5));
    if (thisPath == NULL) {
        printf("malloc failed.\n");
        return NULL;
    } 
    return thisPath;
} 
//create a new path from parent. (have fred)（未来会改用指针数组）
int* newPath(int* parentPath){
    int* thisPath = blankPath();
    memcpy(thisPath,parentPath,sizeof(int)*FLOOR);
    return thisPath;
}
//return the path after add.
int* add(int* path, int zero){
    *(path+step)=zero;
    return path;
}

//以下为流程方法.
//no answer 0 else 1, only when zero is in square[TQS].
int haveAnswer(int square[]){
    int inverseNumber=0; //逆序数.
    int square1[TQS];
    int index_0=0; //0的位置.
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
//only in start.
int zeroOf(int* square){
    for(int i=0;i<TQS;i++){
        if (*(square+i) == 0) return i;
    }
    return -1;
}
//if completed return 1 else 0.
int openingEqualsTo(int square[]){
    for(int i=0;i<TQS-1;i++){
        if (square[i] != openingSquare[i]) return 0;
    }
    return 1;
}
//make an index for each square.
int indexOf(int square[]){
    int number/*顺序数*/, jiecheng/*阶乘*/;
	int result=0;
	for (int i=0;i<8;i++){
        number=0;
		jiecheng=1;
		if(8-i != 0){
		    for(int j=1;j<=8-i;j++){
			    jiecheng*=j;
			}
		}
		for(int j=0;j<=i;j++){
			if (square[j]<square[i]) number+=1;
		}
		result+=jiecheng*(square[i]-number); //获取当前坐标.
	}
    return result;
}
//if visited return 1 else 0 && mark.
int visited(int square[]){
	int indexOfSquare = indexOf(square); 
	if (visitList[indexOfSquare] == 1) return 1;
	else{
		visitList[indexOfSquare] = 1;
		return 0;
	}
}
//look for the neighbors and return a matrix.(warning:手动释放空间!!)
int* neighbourOf(int zero){
    static int neighbour[5] = {-1};
    for (int i = 0, j = 0; i < TQS; i++){
        if(abs(zero%ORDER-i%ORDER)+abs((zero/ORDER-i/ORDER))==1){
            neighbour[j]=i;
            j++;
        } neighbour[j]=-1;
    }
    return neighbour;
}

//以下为输入输出.

//input a mode.
int getMode(){
    char mode= 0;
    printf(">>");
    mode=getche();
    //printf("\n");

    if(mode=='1'){
        while(scanTheSquare3()==-1){
            printf("\n\nREtry:");
        };
        doMode_1();
    }
    else if(mode=='2') doMode_2();  
    else if(mode=='3') doMode_3();
    else if(mode=='4'){
        while(scanTheSquare3()==-1){
            printf("\n\nREtry:");
        };
        doMode_4();
    }
    else if(mode=='?') printf("No help now.\n");
    else return -1;
    return 0;
}
//input a square to begin.
int scanTheSquare3(){
    printf("\ninput the matrix with squares' locations.\n");
    printf("(tips: 0 = blank square; backspace = clear)\n");
    printf("example:\n1 2 3\n4 5 0\n7 8 6\n>>\n");
    char tempChar;
    int tempFig;
    for (int i = 0; i < TQS; i++){
        tempChar=getche();
        while (tempChar < '0' || tempChar >= '0'+TQS){
            if(tempChar=='\b'){
                if(i%ORDER==0) return -1;
                else{
                    i--;
                    printf("\b \b");
                }
            }
            else if(tempChar=='\t') return -1;
            else if(tempChar=='\r'){
                for(int j=0;j<i%ORDER;j++){
                    printf("%d ",openingSquare[i-i%ORDER+j]);
                }
            }
            else printf("\b \b");
            tempChar=getche();
        }
        printf(" ");
        tempFig=tempChar-'0';
        openingSquare[i] = tempFig;
        if(i%ORDER==ORDER-1) printf("\n");
    }
    printf("\n");
    return 0;
}
//mode1: 查找最优解
void doMode_1(){
    node* answer = NULL;
    if(openingEqualsTo(rangeSquare)){
    printf("Have completed.....\n"); //首次完成检查.
    }else if(!haveAnswer(openingSquare)){
        printf("Absolutely have no answer!"); //有解检查.
    }else{
        answer = bfsToFile(newList(),'1');
        if(answer!=NULL){
            printf("Completed!\n");
            printNode(answer);
        }else{
            printf("Error.\n");
        }
        printf("\n");
    }
}
//mode2: 统计最优解分布
void doMode_2(){
    bfsToFile(newList(),'2');
    printf(" _______________ \n");
    printf("|steps\t|amount\t|\n");
    for(int i=0;i<FLOOR;i++){
        printf("|%d\t|%d\t|\n",i+1,stepsList[i]);
    }
    printf(" --------------- \n");
    printf("(9! / 2 = 181440;)\n");
    printf("Have visited %d squares in total.\n\n",totalSquares);
}
//mode3: 统计结果并保存到文件
void doMode_3(){
    FILE* answers_text=fopen("answers.csv","w");
    bfsToFile(newList(),'3');
    printAnswerList(answers_text);
    fclose(answers_text);
}
//print answer of mode 1.
node* printNode(node* node) {
    printf("square =");
    for(int i=0;i<TQS;i++){
        printf(" %d",node->square[i]);
    }
    printf(";\npath of '0' = ");
    for(int i=step;i>=0;i--){
        printf("%d->",node->path[i]+1);
    }
    printf("9;\n");
    printf("%d steps in total.\n", step+1);
    printf("\n");
    return node;
}
//[mode3] save answers in memory.
int arrayToInt(int* array,int lenth){
    int quare=lenth-1;
    int fig=0;
    for(int i=0;i<lenth;i++,quare--){
        fig+=(int)(pow(10,quare)+0.5)*array[i];
    }
    return fig;
}
int append_answer(int square, int id, int* path){
    answers[id].square=square;
    answers[id].step=step;
    for(int i=0;i<10;i++){
        if(answers[id].paths[i]==NULL){
            answers[id].paths[i]=path;
            return 0;
        }
    }
    printf("%d--answer>10",id);
    return -1;
}
//[mode3] print figures in an array one by one to file.
int print_array(int* array, int lenth, FILE* file){
    for(int i=0;i<lenth;i++){
        fprintf(file,"%d",array[i]);
    }
    return 0;
}
//[mode3] print answers to a file.
int printAnswerList(FILE* file){
    int index;
    for(int line=0;line<362881;line++){
        index=0;
        fprintf(file,"%d,%d,%d",line,answers[line].square,answers[line].step);
        while (answers[line].paths[index]!=NULL){
            fprintf(file,",");
            print_array(answers[line].paths[index],answers[line].step,file);
            index++;
        }
        fprintf(file,"\n");
    }
}