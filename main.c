#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define bool int
#define true 1
#define false 0


struct FOOD{
    char name[30];
    int cap, time, progress;
}food[100];

struct COMBO{
    char name[30];
    int count;
    int foodIndex[20];
}combo[100];

struct ORDER{
    int foodIndex[20];
    int count;
    int in, out;
};

FILE* dict;
FILE* input;
FILE* output;

int foodCount, comboCount;


void secPrint(int sec, FILE* output){
    int hour=7+(sec-sec%3600)/3600;
    sec%=3600;
    int min=(sec-sec%60)/60;
    int sec=sec%60;
    fprintf(output, "%d:%d:%d\n", hour, min, sec);
}

int time2sec(char time[9]){
    int hour=(time[0]-'0')*10+time[1]-'0'-7,
        min=(time[3]-'0')*10+time[4]-'0',
        sec=(time[6]-'0')*10+time[7]-'0';
    int foodCount=hour*3600+min*60+sec;
    return foodCount;
}

int searchInFood(char* name){
    for(int i=0;i<foodCount;i++){
        if(strcmp(food[i].name, name)==0) return i;
    }
    return -1;
}

int searchInCombo(char* name){
    for(int i=0;i<foodCount;i++){
        if(strcmp(combo[i].name, name)==0) return i;
    }
    return -1;
}

void dictRead(FILE* dict){
    fscanf(dict, "%d%d\foodCount", &foodCount, &comboCount);
    for(int i=0;i<foodCount;i++) fscanf(dict, "%s", food[i].name);
    for(int i=0;i<comboCount;i++){
        fscanf(dict, "%s", combo[i].name);
        combo[i].count=0;
        for(;;combo[i].count++){
            char name[30];
            fscanf(dict, "%s", name);
            combo[i].foodIndex[combo[i].count]=searchInFood(name);
            int c=fgetc(dict);
            if(c==13||c==10) break;
            if(c==EOF) return;
        }
    }
}

void orderRead(FILE* input, int i, struct ORDER* order){
    order[i].foodIndex={0};
    order[i].out=-1;
    char time[9], name[30];
    fscanf(input, "%8s", &time);
    order[i].in=time2sec(time);
    fscanf(input, "%s", name);
    if(searchInFood(name)!=-1){
        order[i].foodIndex[0]=searchInFood(name);
        order[i].count=1;
    } else{
        memcpy(order[i].foodIndex, combo[searchInCombo(name)].foodIndex, sizeof(order[i].foodIndex));
        order[i].count=combo[searchInCombo(name)].count;
    }
}

int cmp(const void* a, const void* b){
    struct ORDER* orderA=*((struct ORDER*)a);
    struct ORDER* orderB=*((struct ORDER*)b);
    int outA=orderA->out, outB=orderB->out;
    return (outA<outB)-(outA>outB);
}

int w2thBigSec(struct ORDER* order, int i, int w2){
    struct ORDER** orderPtr=(struct ORDER**)malloc(i*sizeof(struct ORDER*));
    for(int j=0;j<=i;j++) *(orderPtr[j])=order[j];
    qsort(orderPtr, i, sizeof(struct ORDER*), cmp);
    return orderPtr[w2]->out;
}

void orderHandle(int w1, int w2, int i, int* curCloseSec, int* curOpenSec, struct ORDER* order){
    int afterInSecCount=0, expectedOut=0;
    bool immediatelyComplete=true;
    for(int j=0;j<order[i].count;j++) if((order[i].in-food[order[i].foodIndex[j]].progress)<food[order[i].foodIndex[j]].time){
        immediatelyComplete=false;
        break;
    }
    if(immediatelyComplete){
        expectedOut=order[i].in;
        for(int j=0;j<order[i].count;j++) food[order[i].foodIndex[j]].progress+=food[order[i].foodIndex[j]].time;
    } else{
        if(order[i].in>*curCloseSec&&order[i].in<*curOpenSec){
            order[i].out=-1;
            return;
        }
        for(int j=0;j<i;j++) if(order[j].out>order[i].in) afterInSecCount++;
        if(afterInSecCount>=(w1-1)){
            *curCloseSec=order[i].in;
            *curOpenSec=1+w2thBigSec(*order, i);
        }
    }
}

void orderOutput(FILE* output, int i, struct ORDER* order){
    if(order[i].out!=-1) secPrint(order[i].out, output);
    else fprintf(output, "Fail\n")
}

void inputRead(FILE* input){
    int orderCount, w1, w2;
    fscanf(input, "%d%d%d", &orderCount, &w1, &w2);
    for(int i=0;i<foodCount;i++) fscanf(dict, "%d", food[i].time);
    for(int i=0;i<foodCount;i++) fscanf(dict, "%d", food[i].cap);
    struct ORDER* order=(struct ORDER*)malloc(orderCount*sizeof(struct ORDER));
    int *curCloseSec=-1, *curOpenSec=-1;
    for(int i=0;i<orderCount;i++){
        orderRead(input, i, *order);
        orderHandle(w1, w2, i, *curCloseSec, *curOpenSec, *order);
        orderOutput(output, i, *order);
    }
    free(order);
}

int main(int argc, char** argv){
    for(int i=0; i<100; i++){
        memset(combo[i].name, 0, sizeof(combo[i].name));
        memset(combo[i].foodIndex, -1, sizeof(combo[i].foodIndex));
        memset(food[i].name, 0, sizeof(food[i].name));
        memset(food[i].progress, 0, sizeof(food[i].progress));
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