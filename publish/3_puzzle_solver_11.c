/*3-PUZZLE SOLVER X
**************************************
*三阶数字华容道求解器，可以
*1 输入一个开局，然后得到一个最优解
*2 统计所有开局的最优解分布情况
*3 保存所有开局的最优解到文件
*4 从文件读取任意开局的最优解
*@Catamint
**************************************
*/
#include <conio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ORDER 3           // 阶数.
#define TQS 9             // square的数量. (ORDER^2)
#define FLOOR 33          // step(步数)的最大值.
#define FACTORIAL 362880  // TQS的阶乘.

typedef struct ListNode {
    char* square;  // [FLOOR] (宫格)
    int zero;      // 空白格子(0)的位置.
    char* path;    // [TQS]
    struct ListNode* next;
    struct ListNode* last;
} node;
struct answer {
    char* square;     // 盘面.
    int step;         // 到达此盘面所用步数.
    char* paths[10];  // "0"的路径(倒序排列).
} answers[FACTORIAL + 1];

int step = 0;                          // 当前层数/步数.
int totalSquares = 0;                  // 已经经过的square数量.
int visitList[FACTORIAL + 1] = {0};    // 存放square是否已经过(1).
int stepsList[FLOOR] = {0};            // 存放步数为[]的数量.
char openingSquare[TQS + 5] = {'\0'};  // 输入的square,即开局.
char rangeSquare[TQS + 5] = "123456780";

//流程：
void inatialization();
int getMode();
int scanTheSquare3();
void doMode_1();
node* printNode(node* node);
void doMode_2();
void doMode_3();
int appendAnswer(char* square, int id, char* path);
int printAnswerList(FILE* file);
int doMode_4();
int splitByComma(char* line, char* ans[]);
node* bfsToFile(node* headNode, char mode);
//链表结构：
node* newList();
node* newNode(char* square, char* path, int zero);
void linkNodeIback(node* parentNode, node* thisNode);
void freeNode(node* thisNode);
void deleteNode(node* thisNode);
//节点结构：
char* newSquare(char* parentSquare);
char* swap(char* square, int zero, int neighbour);
char* newPath(char* parentPath);
char* add(char* path, int zero);
//数据操作：
int haveAnswer(char square[]);
int indexOf(char square[]);
int visited(char square[]);
int* neighbourOf(int zero);

int main() {
    printf("3-PUZZLE SOLVER v4\n");
    do {
        printf("\n");
        printf(
            "[1]=solve a puzzle\t[2]=statistics all\n"
            "[3]=create file\t\t[4]=solve by file\n");
        printf("Press [q] to quit.\n");
        do {
            printf("\b\b\b   \b\b\b>>");
        } while (getMode() == -1);
        inatialization();
    } while (1);
    return 0;
}
//流程：
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
        case 'q':
            exit(0);
        default:
            return -1;
    }
    return 0;
}
// [mode 1] 查找最优解.
void doMode_1() {
    node* answer = NULL;
    while (scanTheSquare3() == -1) {
        printf("\n\nREtry:");
    };  //输入.
    if (strcmp(rangeSquare, openingSquare) == 0) {
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
// 输入开局的square.
int scanTheSquare3() {
    printf("\ninput the matrix with squares' locations.\n");
    printf("(tips: 0 = blank square; backspace = clear)\n");
    printf("example:\n1 2 3\n4 5 0\n7 8 6\n>>\n");
    char tempChar;
    int scanned[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int flagged;
    for (int i = 0; i < TQS; i++) {
        flagged = 1;
        while (flagged) {
            tempChar = getche();
            if (tempChar == '\b') {
                if (i % ORDER == 0)  // 行首退格: 重新输入
                    return -1;
                else {
                    i--;
                    printf("\b \b");  // 行中退格: 退格
                }
            } else if (tempChar == '\t') {
                return -1;
            } else if (tempChar == '\r') {
                for (int j = 0; j < i % ORDER; j++) {
                    printf("%c ", openingSquare[i - i % ORDER + j]);
                }
            } else if (tempChar < '0' || tempChar >= '0' + TQS) {
                printf("\b \b");
            } else if (scanned[(int)tempChar - (int)'0'] == 1) {
                printf("\b \b");
            } else {
                flagged = 0;
                scanned[(int)tempChar - (int)'0'] = 1;
            }
        }
        printf(" ");
        openingSquare[i] = tempChar;
        if (i % ORDER == ORDER - 1) printf("\n");
    }
    printf("\n");
    return 0;
}
// 打印结果.
node* printNode(node* node) {
    printf("square =");
    for (int i = 0; i < TQS; i++) {
        printf(" %c", node->square[i]);
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
// [mode 2] 统计最优解分布.
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
// [mode 3] 统计所有开局的结果并保存到文件.
void doMode_3() {
    FILE* answersFile = fopen("answers.csv", "w");
    bfsToFile(newList(), '3');
    printAnswerList(answersFile);
    fclose(answersFile);
}
// 在answers数组中暂存结果.
int appendAnswer(char* square, int id, char* path) {
    if (answers[id].square == NULL) answers[id].square = square;
    answers[id].step = step + 1;
    for (int i = 0; i < 10; i++) {
        if (answers[id].paths[i] == NULL) {
            answers[id].paths[i] = path;
            return 0;
        }
    }
    printf("%d--more than 10 answers", id);
    return -1;
}
// 把answers保存到文件.
int printAnswerList(FILE* file) {
    int index;
    for (int line = 0; line < FACTORIAL + 1; line++) {
        index = 0;
        fprintf(file, "%d,%s,%d", line, answers[line].square,
                answers[line].step);
        while (answers[line].paths[index] != NULL) {
            fprintf(file, ",");
            fprintf(file, "%s", answers[line].paths[index]);
            index++;
        }
        fprintf(file, "\n");
    }
    for (int i = 0; i < FACTORIAL + 1; i++) {
        for (int j = 0; j < 10; j++) {
            free(answers[i].square);
            free(answers[i].paths[j]);
            answers[i].square = NULL;
            answers[i].paths[j] = NULL;
        }
    }
}
// [mode 4] 从文件读取任意开局的最优解.
int doMode_4() {
    FILE* answersFile = NULL;
    char line[500];
    char* ans[13] = {NULL};
    int index;
    while (scanTheSquare3() == -1) {
        printf("\n\nREtry:");
    };  //输入.
    index = indexOf(openingSquare);
    if ((answersFile = fopen("answers.csv", "r")) == NULL) {
        printf("Faild to open the file.\n");
        return -1;
    }  //打开文件.
    if (strcmp(rangeSquare, openingSquare) == 0) {
        printf("Have completed.....\n");  //首次完成检查.
    } else if (!haveAnswer(openingSquare)) {
        printf("Absolutely have no answer!");  //有解检查.
    } else {
        for (int i = 0; i < index + 1; i++) {
            fgets(line, 500, answersFile);
        }  //指向指定行.

        if (splitByComma(line, ans) == -1) {
            printf("File error!");  //文件中找不到此行.
            return -1;
        }
        printf("ID=%s, square =%s\n", ans[0], ans[1]);
        printf("path of '0':\n");
        for (int i = 3; ans[i]; i++) {
            char* p = ans[i];
            while (*(++p) != '\0')
                ;
            while (p-- != ans[i]) printf("%c->", (*p) + 1);
            printf("9\n");
        }
        printf("%s steps in total.\n\n", ans[2]);
    }
}
// 在逗号处分割字符串.
int splitByComma(char* line, char* ans[]) {
    char* p = line;
    int i = 0;
    ans[0] = strtok(line, ",\n");
    while ((ans[++i] = strtok(NULL, ",\n")) && i < 13)
        ;
    if (i < 3 || i == 12) return -1;
    return 0;
}

// 数据初始化
void inatialization() {
    step = 0;          //全局层数.
    totalSquares = 0;  //全局组合数.
    for (int i = 0; i < FACTORIAL + 1; i++) {
        visitList[i] = 0;
    }
    for (int i = 0; i < FLOOR; i++) {
        stepsList[i] = 0;
    }
}
// 宽度搜索3
node* bfsToFile(node* headNode, char mode) {
    node* thisNode = headNode->next;
    int* neighbour = NULL;
    node* tempNode = NULL;
    char* tempSquare = NULL;
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
                if (mode == '3' &&
                    step + 1 == answers[indexOf(tempSquare)].step) {
                    appendAnswer(tempSquare, indexOf(tempSquare),
                                 add(newPath(thisNode->path), *neighbour));
                } else
                    free(tempSquare);
                continue;
            }
            tempNode =
                newNode(tempSquare, add(newPath(thisNode->path), *neighbour),
                        *neighbour);
            linkNodeIback(thisNode, tempNode);  //
            if (mode == '1' && strcmp(tempSquare, openingSquare) == 0)
                return tempNode;  // return the answer.
            else if (mode == '2') {
                totalSquares++;
                stepsList[step]++;
            } else if (mode == '3') {
                appendAnswer(tempSquare, indexOf(tempSquare), tempNode->path);
            }
        } while (*(++neighbour) != -1);  //
        if (mode == '1' || mode == '2') {
            free(thisNode->square);
            free(thisNode->path);
        }
        thisNode = thisNode->next;
        deleteNode(thisNode->last);
    } while (step < FLOOR);
    return NULL;
}

//以下为数据操作.
// 判断开局是否有解.
int haveAnswer(char square[]) {
    int inverseNumber = 0;  //逆序数.
    int square1[TQS];
    int indexOfZero = 0;  // 0的位置.
    for (int i = 0; i < TQS; i++) {
        /*square类型改为int,并将0改为TQS*/
        int current = (int)square[i] - '0';  // atoi.
        if (current == 0) {
            square1[i] = TQS;
            indexOfZero = i;
        } else {
            square1[i] = current;
        }
    }
    for (int i = 0; i < TQS; i++) {
        for (int j = 0; j < i; j++) {
            if (square1[j] > square1[i]) inverseNumber++;
        }
    }
    if ((inverseNumber + indexOfZero) % 2 == 0)
        return 1; //有解.
    else
        return 0; //无解.
}
// 为每个square编特定号码.
int indexOf(char square[]) {
    int number /*顺序数*/, jiecheng /*阶乘*/;
    int result = 0;
    for (int i = 0; i < 8; i++) {
        int current = (int)square[i] - '0';  // atoi
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
        result += jiecheng * (current - number);  // 获取当前坐标.
    }
    return result;
}
// 判断square是否已经过.
int visited(char square[]) {
    int indexOfSquare = indexOf(square);
    if (visitList[indexOfSquare] == 1)
        return 1;
    else {
        visitList[indexOfSquare] = 1;
        return 0;
    }
}
// 寻找zero的相邻方块.
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

//以下为链表结构及操作.
// 创建新链表.
node* newList() {
    node* head = newNode(NULL, NULL, -1);
    node* firstNode = newNode(rangeSquare, newPath(""), TQS - 1);  //
    head->last = firstNode;
    head->next = firstNode;
    firstNode->last = head;
    firstNode->next = head;  //实现循环
    return head;
}
// new a node
node* newNode(char* square, char* path, int zero) {
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
// 在后面连接节点.
void linkNodeIback(node* parentNode, node* thisNode) {
    if (parentNode->last != NULL) parentNode->last->next = thisNode;
    thisNode->last = parentNode->last;
    thisNode->next = parentNode;
    parentNode->last = thisNode;
}
// 删除节点.
void deleteNode(node* thisNode) {
    if (thisNode != NULL) {
        node* nextNode = thisNode->next;
        node* lastNode = thisNode->last;
        lastNode->next = nextNode;
        if (nextNode != NULL) nextNode->last = lastNode;
        free(thisNode);
        thisNode = NULL;
    }
}

//以下为节点内部结构及操作.
// 创建新square并赋值.
char* newSquare(char* parentSquare) {
    char* thisSquare = (char*)calloc(TQS + 5, sizeof(char));
    if (thisSquare == NULL) {
        printf("malloc failed.\n");
        return NULL;
    }
    strcpy(thisSquare, parentSquare);
    return thisSquare;
}
// 返回对换后的square.
char* swap(char* square, int zero, int neighbour) {
    char temp = *(square + zero);
    *(square + zero) = *(square + neighbour);
    *(square + neighbour) = temp;
    return square;
}
// new a path by args. (have fred)
char* newPath(char* parentPath) {
    char* thisPath = (char*)malloc(sizeof(int) * (FLOOR + 5));
    if (thisPath == NULL) {
        printf("malloc failed.\n");
        return NULL;
    }
    strcpy(thisPath, parentPath);
    return thisPath;
}
// 返回添加末尾后的path.
char* add(char* path, int zero) {
    *(path + step) = (char)(zero + '0');
    *(path + step + 1) = '\0';
    return path;
}
