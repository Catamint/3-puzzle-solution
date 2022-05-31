#include<stdio.h>
#define TQS 9
int visitList[362881] = { 0 };
int indexOf(int square[]){
    int number/*顺序数*/, jiecheng;
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
int haveAnswer(int square[]){
    int inverseNumber=0;
    int square1[TQS];
    for (int i = 0; i < TQS; i++)
    {
        if(square[i]==0) square1[i]=TQS;
        else square1[i]=square[i];
    }
    for(int i=0;i<TQS;i++){
        for(int j=0;j<i;j++){
            if(square1[j]>square1[i]) inverseNumber++;
        }
    }
    if(inverseNumber%2==0) return 1;
    else return 0;
}
int main(){
    int square[TQS]={1,2,3,4,5,6,0,7,8};
    printf("%d\n",haveAnswer(square));

    //visited(square);
    getchar();
}