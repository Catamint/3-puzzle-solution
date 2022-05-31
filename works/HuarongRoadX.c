/*3-PUZZLE SOLVER X
**************************************
*三阶数字华容道求解器，可以
*1 输入一个开局，然后得到一个最优解
*2 统计所有开局的最优解分布情况
*3 保存所有开局的最优解到文件
*4 从文件读取任意开局的最优解
*@Catamint
**************************************
已知的问题:
1.square转为整数后储存,开头的0被抹去了
2.paths没有free
3.mode1只能输出一个结果
**************************************
*/

#include <conio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <time.h>

#define ORDER 3           // 阶数.
#define TQS 9             // square的数量. (ORDER^2)
#define FLOOR 33          // step(步数)的最大值.
#define FACTORIAL 362880  // TQS的阶乘.

typedef struct ListNode {
    int* square;  // [FLOOR] (宫格)
    int zero;     // Location of blank square.
    char* path;   // [TQS]
    struct ListNode* next;
    struct ListNode* last;
} node;

struct answer {
    int square;       // 盘面.
    int step;         // 到达此盘面所用步数.
    char* paths[10];  // "0"的路径(倒序排列).
} answers[FACTORIAL + 1];

int step = 0;                        // 当前层数/步数.
int totalSquares = 0;                // amount of squares been visited.
int visitList[FACTORIAL + 1] = {0};  // if visited turn to 1.
int stepsList[FLOOR] = {0};          // 存放步数为[]的数量.
int openingSquare[TQS];              // 输入的square,即开局.
int rangeSquare[TQS] = {1, 2, 3, 4, 5, 6, 7, 8, 0};

//流程：
void inatialization();
int getMode();
int scanTheSquare3();
void doMode_1();
node* printNode(node* node);
void doMode_2();
void doMode_3();
int appendAnswer(int square, int id, char* path);
int printAnswerList(FILE* file);
int doMode_4();
int splitByComma(char* line, char ans[][FLOOR * 5]);
int arrayToInt(int* array, int lenth);
node* bfsToFile(node* headNode, char mode);
//链表结构：
node* newList();
node* newNode(int* square, char* path, int zero);
void LinkNodeIback(node* parentNode, node* thisNode);
void deleteNode(node* thisNode);
//节点结构：
int* blankSquare();
int* newSquare(int* parentSquare);
int* swap(int* square, int zero, int neighbour);
char* blankPath();
char* newPath(char* parentPath);
char* add(char* path, int zero);
//数据操作：
int haveAnswer(int square[]);
int zeroOf(int* square);
int openingEqualsTo(int* square);
int indexOf(int square[]);
int visited(int square[]);
int* neighbourOf(int zero);

void play(){

}
// only in start.
int zeroOf(int* square) {
    for (int i = 0; i < TQS; i++) {
        if (*(square + i) == 0) return i;
    }
    return -1;
}
void demo(int* square, char* path){

}

int main() {
    printf("3-PUZZLE SOLVER v4\n");
    do {
        printf("\n");
        printf(
            "[1]=solve a puzzle\t[2]=statistics all\n"
            "[3]=create file\t\t[4]=solve by file\n");
        printf("Press [?] for help; [q] to quit.\n");
        do {
            printf("\b\b\b   \b\b\b>>");
        } while (getMode() == -1);
        inatialization();
    } while (1);
    return 0;
}

//查找最优解.
void doMode_1() {
    node* answer = NULL;
    while (scanTheSquare3() == -1) {
        printf("\n\nREtry:");
    };  //输入.
    if (openingEqualsTo(rangeSquare)) {
        printf("Have completed.....\n");  //首次完成检查.
    } else if (!haveAnswer(openingSquare)) {
        printf("Absolutely have no answer!");  //有解检查.
    } else {
        answer = bfsToFile(newList(), '1');  //进入运算.
        if (answer != NULL) {
            printf("Completed!\n");
            printNode(answer);
        } else {
            printf("Input error.\n");
        }
        printf("\n");
    }
}
//[mode 1] 打印结果.
node* printNode(node* node) {
    printf("square =");
    for (int i = 0; i < TQS; i++) {
        printf(" %d", node->square[i]);
    }
    printf(";\npath of '0' = ");
    for (int i = step; i >= 0; i--) {
        printf("%c->", node->path[i] + 1);
    }
    printf("9;\n");
    printf("%d steps in total.\n", step + 1);
    printf("\n");
    return node;
}

//统计最优解分布.
void doMode_2() {
    bfsToFile(newList(), '2');
    printf("\n _______________ \n");
    printf("|steps\t|amount\t|\n");
    for (int i = 0; i < FLOOR; i++) {
        printf("|%d\t|%d\t|\n", i + 1, stepsList[i]);
    }
    printf(" --------------- \n");
    printf("(9! / 2 = 181440;)\n");
    printf("Have visited %d squares in total.\n\n", totalSquares);
}

//统计所有开局的结果并保存到文件.
void doMode_3() {
    FILE* answersFile = fopen("answers.csv", "w");
    bfsToFile(newList(), '3');
    printAnswerList(answersFile);
    fclose(answersFile);
}
//[mode3] save answers in memory.
int appendAnswer(int square, int id, char* path) {
    answers[id].square = square;
    answers[id].step = step+1;
    for (int i = 0; i < 10; i++) {
        if (answers[id].paths[i] == NULL) {
            answers[id].paths[i] = path;
            return 0;
        }
    }
    printf("%d--more than 10 answers", id);
    return -1;
}
//[mode3] 把answers保存到文件.
int printAnswerList(FILE* file) {
    int index;
    for (int line = 0; line < FACTORIAL + 1; line++) {
        index = 0;
        fprintf(file, "%d,%d,%d", line, answers[line].square,
                answers[line].step);
        while (answers[line].paths[index] != NULL) {
            fprintf(file, ",");
            fprintf(file, "%s", answers[line].paths[index]);
            index++;
        }
        fprintf(file, "\n");
    }
}
//[mode 3] 
int arrayToInt(int* array, int lenth) {
    int quare = lenth - 1;
    int fig = 0;
    for (int i = 0; i < lenth; i++, quare--) {
        fig += (int)(pow(10, quare) + 0.5) * array[i];
    }
    return fig;
}

// 从文件读取任意开局的最优解.
int doMode_4() {
    FILE* answersFile = NULL;
    char line[500];
    char ans[13][FLOOR * 5];
    int amountOfAns;
    int index;
    while (scanTheSquare3() == -1) {
        printf("\n\nREtry:");
    };  //输入.
    index = indexOf(openingSquare);
    if ((answersFile = fopen("answers.csv", "r")) == NULL) {
        printf("Faild to open the file.\n");
        return -1;
    }  //打开文件.
    if (openingEqualsTo(rangeSquare)) {
        printf("Have completed.....\n");  //首次完成检查.
    } else if (!haveAnswer(openingSquare)) {
        printf("Absolutely have no answer!");  //有解检查.
    } else {
        for (int i = 0; i < index + 1; i++) {
            fgets(line, 500, answersFile); 
        }  //指向指定行.
        if ((amountOfAns = splitByComma(line, ans)) == -1) return -1;
        printf("ID=%s, square =%s\n", ans[0], ans[1]);
        printf("path of '0':\n");
        for (int i = 3; i < amountOfAns; i++) {
            char* p = ans[i];
            while (*(++p) != '\0')
                ;
            while (p-- != ans[i])
                printf("%c->", (*p) + 1);
            printf("9\n");
        }
        printf("%s steps in total.\n\n", ans[2]);
    }
}
//[mode 4]在逗号处分割字符串.
int splitByComma(char* line, char ans[][FLOOR * 5]) {
    char* p = line;
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < FLOOR; j++, p++) {
            if (*p == ',') {
                ans[i][j] = '\0';
                p++;
                break;
            } else if (*p == '\n') {
                ans[i][j] = '\0';
                return i + 1;
            } else {
                ans[i][j] = *p;
            }
        }
    }
    return -1;
}

void help() {}

//数据初始化
void inatialization() {
    step = 0;          //全局层数.
    totalSquares = 0;  //全局组合数.
    for (int i = 0; i < FACTORIAL + 1; i++) {
        visitList[i] = 0;
    }
    for (int i = 0; i < FLOOR; i++) {
        stepsList[i] = 0;
    }
    for (int i = 0; i < FACTORIAL + 1; i++) {
        for (int j = 0; j < 10; j++) {
            answers[i].paths[j] = NULL;
        }
    }
}
//宽度搜索3
node* bfsToFile(node* headNode, char mode) {
    node* thisNode = headNode->next;
    int* neighbour = NULL;
    node* tempNode = NULL;
    int* tempSquare = NULL;
    do {
        if (thisNode == headNode) {
            if (thisNode->next == thisNode) return NULL;
            thisNode = thisNode->next;
            step++;
            continue;
        }
        neighbour = neighbourOf(thisNode->zero);
        do {
            tempSquare =
                swap(newSquare(thisNode->square), thisNode->zero, *neighbour);
            if (visited(tempSquare)) {
                if (mode == '3' && step + 1 == answers[indexOf(tempSquare)].step) {
                    appendAnswer(arrayToInt(tempSquare, TQS),
                                  indexOf(tempSquare),
                                  add(newPath(thisNode->path), *neighbour));
                }
                free(tempSquare);
                continue;
            }
            tempNode =
                newNode(tempSquare, add(newPath(thisNode->path), *neighbour),
                        *neighbour);
            LinkNodeIback(thisNode, tempNode);  //
            if (mode == '1' && openingEqualsTo(tempSquare))
                return tempNode;  // return the answer.
            else if (mode == '2') {
                totalSquares++;
                stepsList[step]++;
            } else if (mode == '3') {
                appendAnswer(arrayToInt(tempSquare, TQS),
                              indexOf(tempSquare), tempNode->path);
            }
        } while (*(++neighbour) != -1);  //
        thisNode = thisNode->next;
        deleteNode(thisNode->last);
    } while (step < FLOOR);
    return NULL;
}

//以下为链表结构及操作.
// Create a new list and get the initial data (by arg).
node* newList() {
    node* head = newNode(NULL, NULL, -1);
    node* firstNode = newNode(rangeSquare, blankPath(), TQS - 1);
    *(firstNode->path) = (char)(firstNode->zero + '0');
    *(firstNode->path + 1) = '\0';  ////////////////////////////////
    head->last = firstNode;
    head->next = firstNode;
    firstNode->last = head;
    firstNode->next = head;  //实现循环
    return head;
}
// new a node
node* newNode(int* square, char* path, int zero) {
    node* thisNode = (node*)malloc(sizeof(node));
    if (thisNode == NULL) {
        printf("malloc failed.\n");
        return NULL;
    }
    thisNode->last = NULL;
    thisNode->next = NULL;
    thisNode->square = square;
    thisNode->path = path;
    thisNode->zero = zero;
    return thisNode;
}
// link a node in the back of parent.
void LinkNodeIback(node* parentNode, node* thisNode) {
    if (parentNode->last != NULL) parentNode->last->next = thisNode;
    thisNode->last = parentNode->last;
    thisNode->next = parentNode;
    parentNode->last = thisNode;
}
// delete the node.
void deleteNode(node* thisNode) {
    if (thisNode != NULL) {
        node* nextNode = thisNode->next;
        node* lastNode = thisNode->last;
        lastNode->next = nextNode;
        if (nextNode != NULL) nextNode->last = lastNode;
        free(thisNode->square);
        // free(thisNode->path);  //(mode3需要paths,不free了)/////////(结束时free)
        free(thisNode);
        thisNode = NULL;
    }
}

//以下为node内部结构及操作.
// create a blank square.
int* blankSquare() {
    int* thisSquare = (int*)malloc(sizeof(int) * TQS);
    if (thisSquare == NULL) {
        printf("malloc failed.\n");
        return NULL;
    }
    return thisSquare;
}
// create a new Square from parent.
int* newSquare(int* parentSquare) {
    int* thisSquare = blankSquare();
    memcpy(thisSquare, parentSquare, sizeof(int) * TQS);
    return thisSquare;
}
// return the square after swapping(对换).
int* swap(int* square, int zero, int neighbour) {
    int temp = *(square + zero);
    *(square + zero) = *(square + neighbour);
    *(square + neighbour) = temp;
    return square;
}
// create a blank path. (have fred)
char* blankPath() {
    char* thisPath = (char*)malloc(sizeof(int) * (FLOOR + 5));
    if (thisPath == NULL) {
        printf("malloc failed.\n");
        return NULL;
    }
    return thisPath;
}
// create a new path from parent. (have fred)
char* newPath(char* parentPath) {
    char* thisPath = blankPath();
    strcpy(thisPath, parentPath);
    return thisPath;
}
// return the path after add.
char* add(char* path, int zero) {
    *(path + step) = (char)(zero + '0');
    *(path + step + 1) = '\0';
    return path;
}

//以下为流程方法.
// no answer 0 else 1, only when zero is in square[TQS].
int haveAnswer(int square[]) {
    int inverseNumber = 0;  //逆序数.
    int square1[TQS];
    int indexOfZero = 0;  // 0的位置.
    for (int i = 0; i < TQS; i++) {
        if (square[i] == 0) {
            square1[i] = TQS;
            indexOfZero = i;
        } else {
            square1[i] = square[i];
        }
    }
    for (int i = 0; i < TQS; i++) {
        for (int j = 0; j < i; j++) {
            if (square1[j] > square1[i]) inverseNumber++;
        }
    }
    if ((inverseNumber + indexOfZero) % 2 == 0)
        return 1;
    else
        return 0;
}
// if completed return 1 else 0.
int openingEqualsTo(int square[]) {
    for (int i = 0; i < TQS - 1; i++) {
        if (square[i] != openingSquare[i]) return 0;
    }
    return 1;
}
// make an index for each square.
int indexOf(int square[]) {
    int number /*顺序数*/, jiecheng /*阶乘*/;
    int result = 0;
    for (int i = 0; i < 8; i++) {
        number = 0;
        jiecheng = 1;
        if (8 - i != 0) {
            for (int j = 1; j <= 8 - i; j++) {
                jiecheng *= j;
            }
        }
        for (int j = 0; j <= i; j++) {
            if (square[j] < square[i]) number += 1;
        }
        result += jiecheng * (square[i] - number);  //获取当前坐标.
    }
    return result;
}
// if visited return 1 else 0 && mark.
int visited(int square[]) {
    int indexOfSquare = indexOf(square);
    if (visitList[indexOfSquare] == 1)
        return 1;
    else {
        visitList[indexOfSquare] = 1;
        return 0;
    }
}
// look for the neighbors and return a matrix.(warning:手动释放空间!!)
int* neighbourOf(int zero) {
    static int neighbour[5] = {-1};
    for (int i = 0, j = 0; i < TQS; i++) {
        if (abs(zero % ORDER - i % ORDER) + abs((zero / ORDER - i / ORDER)) ==
            1) {
            neighbour[j] = i;
            j++;
        }
        neighbour[j] = -1;
    }
    return neighbour;
}

//以下为输入输出.

// input a mode.
int getMode() {
    char mode = getche();
    switch (mode) {
        case '1':
            doMode_1(); break;
        case '2':
            doMode_2(); break;
        case '3':
            doMode_3(); break;
        case '4':
            doMode_4(); break;
        case '?':
            printf("No help now.\n"); break;
        case 'q':
            exit(0);
        default:
            return -1;
    }
    return 0;
}
// input a square to begin.
int scanTheSquare3() {
    printf("\ninput the matrix with squares' locations.\n");
    printf("(tips: 0 = blank square; backspace = clear)\n");
    printf("example:\n1 2 3\n4 5 0\n7 8 6\n>>\n");
    char tempChar;
    int tempFig;
    for (int i = 0; i < TQS; i++) {
        tempChar = getche();
        while (tempChar < '0' || tempChar >= '0' + TQS) {
            if (tempChar == '\b') {
                if (i % ORDER == 0)
                    return -1;
                else {
                    i--;
                    printf("\b \b");
                }
            } else if (tempChar == '\t')
                return -1;
            else if (tempChar == '\r') {
                for (int j = 0; j < i % ORDER; j++) {
                    printf("%d ", openingSquare[i - i % ORDER + j]);
                }
            } else
                printf("\b \b");
            tempChar = getche();
        }
        printf(" ");
        tempFig = tempChar - '0';
        openingSquare[i] = tempFig;
        if (i % ORDER == ORDER - 1) printf("\n");
    }
    printf("\n");
    return 0;
}
