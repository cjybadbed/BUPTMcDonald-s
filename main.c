#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define bool int
#define true 1
#define false 0


struct FOOD{
    char name[30];
    int cap, time, captime;  //captime is when the storage limit is met.
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

typedef struct FOOD FOOD;
typedef struct COMBO COMBO;
typedef struct ORDER ORDER;

FILE* dict;
FILE* input;
FILE* output;

int foodCount, comboCount, orderCount;

static ORDER* order;

void secPrint(int sec, FILE* output){
    int hour=7+(sec-sec%3600)/3600;
    sec%=3600;
    int min=(sec-sec%60)/60;
    sec%=60;
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

void orderRead(FILE* input){
    for(int i=0;i<orderCount;i++){
        for(int j=0;j<20;j++) order[i].foodIndex[j]=-1;
        order[i].out=0;
        char time[9]={0}, orderName[30]={0};
        fscanf(input, "%s %s", time, orderName);
        order[i].in=time2sec(time);
        int temp=searchInFood(orderName);
        if(temp!=-1){
            order[i].count=1;
            order[i].foodIndex[0]=temp;
        } else{
            temp=searchInCombo(orderName);
            for(int j=0;j<combo[temp].count;j++) order[i].foodIndex[j]=combo[temp].foodIndex[j];
            order[i].count=combo[temp].count;
        }
    }
}

int cmp(const void* a, const void* b){
    struct ORDER* orderA=(struct ORDER*)a;
    struct ORDER* orderB=(struct ORDER*)b;
    int outA=orderA->out, outB=orderB->out;
    return (outA<outB)-(outA>outB);
}

int w2thBigSec(int i, int w2){
    struct ORDER** orderPtr=(struct ORDER**)malloc(i*sizeof(struct ORDER*));
    for(int j=0;j<=i;j++) orderPtr[j]=&(order[j]);
    qsort(orderPtr, i, sizeof(struct ORDER*), cmp);
    return orderPtr[w2-1]->out;
}

bool isImmediateComplete(int i){
    for(int j=0;j<order[i].count;j++){
        FOOD* curr=food[order[i].foodIndex[j]];
        if(curr->captime-(curr->cap-1)*curr->time>order[i].in) return false;
    }
    return true;
}

void orderHandle(int w1, int w2, int* curCloseSec, int* curOpenSec){  //curOpenSec=w2orderOutSec+1
    for(int i=0;i<orderCount;i++){
        if(order[i].in>curCloseSec&&order[i].in<curOpenSec&&!isImmediateComplete(i)){
            order[i].out=-1;
            continue;
        }
        if(isImmediateComplete(i)){
            order[i].out=order[i].in;
            continue;
        }
        int afterCount=0;  //count orders that complete after current order.in, namely currently unfinished orders
        for(int j=0;j<i;j++) if(order[j].out>order[i].in) afterCount++;
        int out=0;
        for(int j=0;j<order[i].count;j++){
            FOOD* curr=food[order[i].foodIndex[j]];
            int n=curr->captime-(curr->cap-1)*curr->time; //n is the first one's completed second.
            if(n>order[i].in&&out<n) out=n;
            if(order[i].in<=curr->captime) curr->captime+=curr->time;
            else curr->captime=order[i].in+curr->time;
            order[i].out=out;
        }
        if(afterCount+1==w1){
            *curCloseSec=order[i].in;
            *curOpenSec=w2thBigSec(i, w2);
        }
    }
}

void orderOutput(FILE* output, int i){
    if(order[i].out!=-1) secPrint(order[i].out, output);
    else fprintf(output, "Fail\n");
}

void inputRead(FILE* input){
    int w1, w2;
    fscanf(input, "%d%d%d", &orderCount, &w1, &w2);
    for(int i=0;i<foodCount;i++) fscanf(input, "%d", &(food[i].time));
    for(int i=0;i<foodCount;i++) fscanf(input, "%d", &(food[i].cap));
    order=(ORDER*)malloc(orderCount*sizeof(ORDER));
    int closeSec=-1, openSec=-1;
    orderRead(input);
    orderHandle(w1, w2, &closeSec, &openSec);
    orderOutput(output);
}

int main(int argc, char** argv){
    for(int i=0; i<100; i++){
        memset(combo[i].name, 0, sizeof(combo[i].name));
        memset(combo[i].foodIndex, -1, sizeof(combo[i].foodIndex));
        memset(food[i].name, 0, sizeof(food[i].name));
        food[i].captime=food[i].cap*food[i].time;
    }
    dict=fopen("dict.dic", "r");
    input=fopen("input.txt", "r");
    output=fopen("output.txt", "a");
    dictRead(dict);
    inputRead(input);
    fclose(dict);
    fclose(input);
    fclose(output);
    free(order);
    return 0;
}