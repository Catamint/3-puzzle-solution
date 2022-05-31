#include<stdio.h>
#include<stdlib.h> 
#include<math.h>
#include<string.h>

int arrayToInt(int* array, int lenth){
    int fig=0;
    int index=lenth-1;
    for(int i=0;i<lenth;i++){
        fig+=(int)(pow(10,index)+0.5)*array[i];
        index--;
    }
    return fig;
}
int split(char* line,char list[][12]){
    char* head=line;
    char* end=line-1;
    int amount_of_paths=0;
    while (*end++!='\n'){
        head=end;
        for(int i=0;i<33+1;i++,end++){
            if(*end==','||*end=='\n'){
                memcpy(list[amount_of_paths++],head,i); 
                break;
            }
        }
    }
    return amount_of_paths;
}

int main(){
    int a[9]={9,8,7,6,5,4,3,2,1};
    char* b="123,123123,123123123,123123123123,123,123\n";
    char c[120][12];
    int amount=split(b,c);
    for (int i = 0; i < amount; i++)
    {
        printf("%s\n",c[i]);
    }
    getchar();
}