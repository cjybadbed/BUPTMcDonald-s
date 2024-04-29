#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct FOOD{
    char name[30];
    int cap, time, progress;
}food[100];

struct COMBO{
    char name[30];
    int count;
    int foodIndex[20];
}combo[100];


FILE* dict;
FILE* input;
FILE* output;

void secPrint(int n){
    int hour=7+(n-n%3600)/3600;
    n%=3600;
    int min=(n-n%60)/60;
    int sec=n%60;
    printf("%d:%d:%d", hour, min, sec);
}

int time2sec(char time[9]){
    int hour=(time[0]-'0')*10+time[1]-'0'-7,
        min=(time[3]-'0')*10+time[4]-'0',
        sec=(time[6]-'0')*10+time[7]-'0';
    int n=hour*3600+min*60+sec;
    return n;
}

int search(char* name, int n){
    for(int i=0;i<n;i++){
        if(strcmp(food[i].name, name)==0) return i;
    }
    return -1;
}

void dictRead(FILE*dict){
    int n, m;
    fscanf(dict, "%d%d\n", &n, &m);
    for(int i=0;i<n;i++) fscanf(dict, "%s", food[i].name);
    for(int i=0;i<m;i++){
        fscanf(dict, "%s", combo[i].name);
        combo[i].count=0;
        for(;;combo[i].count++){
            char name[30];
            fscanf(dict, "%s", name);
            combo[i].foodIndex[combo[i].count]=search(name, n);
            int c=fgetc(dict);
            if(c==13||c==10) break;
            if(c==EOF) return;
        }
    }
}

int main(int argc, char** argv){
    for(int i=0; i<100; i++){
        memset(combo[i].name, 0, sizeof(combo[i].name));
        memset(combo[i].foodIndex, -1, sizeof(combo[i].foodIndex));
    }
    for(int i=0; i<100; i++){
        memset(food[i].name, 0, sizeof(food[i].name));
    }
    dict=fopen("dict.dic", "r");
    input=fopen("input.txt", "r");
    output=fopen("output.txt", "a");
    dictRead(dict);
    printf("%s %s", combo[1].name, food[combo[4].foodIndex[2]].name);
    fclose(dict);
    fclose(input);
    fclose(output);
    return 0;
}