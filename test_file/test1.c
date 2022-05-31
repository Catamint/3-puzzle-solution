#include<stdio.h>
int main(){
    FILE *file1=fopen("file1.txt", "r");
    FILE *file2=fopen("file2.txt","w");
    char char1;
    while ((char1=fgetc(file1))!=EOF){
        fputc(char1,file2);
    }
    rewind(file2);
    fputs("goodby",file2);
    fclose(file1);
    fclose(file2);
}