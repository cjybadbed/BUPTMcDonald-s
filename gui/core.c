#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#define MAX_FOOD 100
#define MAX_COMBO 100
#define MAX_ORDER 200

struct FOOD{
    char name[51];
    int cap, time, captime;  //captime is when the storage limit is met.
};

struct COMBO{
    char name[51];
    int count;
    int foodIndex[20];
};

struct ORDER{
    char name[51];
    int foodIndex[20];
    int count;
    int in, out;
};

typedef struct FOOD FOOD;
typedef struct COMBO COMBO;
typedef struct ORDER ORDER;

extern FOOD food[MAX_FOOD];
extern COMBO combo[MAX_COMBO];
extern ORDER order[MAX_ORDER];

extern FILE* dict;
extern FILE* input;
extern FILE* output;

extern int foodCount, comboCount, orderCount, w1, w2, closeSec, openSec;


void secPrint(int sec, FILE* output){
    int hour=7+(sec-sec%3600)/3600;
    sec%=3600;
    int min=(sec-sec%60)/60;
    sec%=60;
    fprintf(output, "%02d:%02d:%02d\n", hour, min, sec);
}

int time2sec(char time[9]){
    int n;
    int hour=(time[0]-'0')*10+time[1]-'0'-7,
        min=(time[3]-'0')*10+time[4]-'0',
        sec=(time[6]-'0')*10+time[7]-'0';
    n=hour*3600+min*60+sec;
    return n;
}

char* sec2time(int sec){
    static char time[9];
    if(sec>=time2sec("23:59:59")){
        sprintf(time, "23:59:59");
        return time;
    }
    int hour=7+(sec-sec%3600)/3600;
    sec%=3600;
    int min=(sec-sec%60)/60;
    sec%=60;
    sprintf(time, "%02d:%02d:%02d", hour, min, sec);
    return time;
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
    fscanf(dict, "%d%d", &foodCount, &comboCount);
    for(int i=0;i<foodCount;i++) fscanf(dict, "%s", food[i].name);
    for(int i=0;i<foodCount;i++) fscanf(dict, "%d", &(food[i].time));
    for(int i=0;i<foodCount;i++) fscanf(dict, "%d", &(food[i].cap));
    fscanf(dict, "%d%d", &w1, &w2);
    for(int i=0;i<foodCount;i++) food[i].captime=food[i].cap*food[i].time;
    for(int i=0;i<comboCount;i++){
        fscanf(dict, "%s", combo[i].name);
        combo[i].count=0;
        for(;;combo[i].count++){
            char name[50];
            fscanf(dict, "%s", name);
            combo[i].foodIndex[combo[i].count]=searchInFood(name);
            int c=fgetc(dict);
            if(c==13||c==10){
                combo[i].count++;
                break;
            }
            if(c==EOF){
                combo[i].count++;
                return;
            }
        }
    }
}

bool singleOrderRead(int curr_time){
    fpos_t pos;
    fgetpos(input, &pos);
    if(fgetc(input)==EOF){
        fsetpos(input, &pos);
        return 0;
    }
    fsetpos(input, &pos);
    char time[9]={0}, orderName[51]={0};
    fscanf(input, "%s %s", time, orderName);
    if(time2sec(time)!=curr_time){
        fsetpos(input, &pos);
        return 0;
    }
    for(int j=0;j<20;j++) order[orderCount].foodIndex[j]=-1;
    order[orderCount].out=0;
    strcpy(order[orderCount].name, orderName);
    order[orderCount].in=time2sec(time);
    int temp=searchInFood(orderName);
    if(temp!=-1){
        order[orderCount].count=1;
        order[orderCount].foodIndex[0]=temp;
    } else{
        temp=searchInCombo(orderName);
        for(int j=0;j<combo[temp].count;j++) order[orderCount].foodIndex[j]=combo[temp].foodIndex[j];
        order[orderCount].count=combo[temp].count;
    }
    orderCount++;
    return 1;
}

void _singleOrderRead(char* input){
    for(int j=0;j<20;j++) order[orderCount].foodIndex[j]=-1;
    order[orderCount].out=0;
    char time[9]={0}, orderName[51]={0};
    sscanf(input, "%s %s", time, orderName);
    strcpy(order[orderCount].name, orderName);
    order[orderCount].in=time2sec(time);
    int temp=searchInFood(orderName);
    if(temp!=-1){
        order[orderCount].count=1;
        order[orderCount].foodIndex[0]=temp;
    } else{
        temp=searchInCombo(orderName);
        for(int j=0;j<combo[temp].count;j++) order[orderCount].foodIndex[j]=combo[temp].foodIndex[j];
        order[orderCount].count=combo[temp].count;
    }
    orderCount++;
}

int cmp(const void* a, const void* b){
    return (*(int*)a<*(int*)b)-(*(int*)a>*(int*)b);
}

int w2thBigSec(int i){ //too lazy to optimize further...
    int* sec=(int*)malloc(i*sizeof(int));
    for(int j=0;j<i;j++) sec[j]=order[j].out;
    qsort(sec, i, sizeof(int), cmp);
    int result=sec[w2-1];
    free(sec);
    return result;
}

bool isImmediateComplete(int i){
    for(int j=0;j<order[i].count;j++){
        FOOD* curr=&food[order[i].foodIndex[j]];
        if(curr->captime-(curr->cap-1)*curr->time>=order[i].in) return false;
    }
    return true;
}

void ithOrderHandle(int i){  //curOpenSec=w2orderOutSec+1
    if((order[i].in>closeSec&&order[i].in<openSec)||order[i].in>time2sec("22:00:00")){
        order[i].out=-1;
        return;
    }
    if(isImmediateComplete(i)){
        order[i].out=order[i].in;
        for(int j=0;j<order[i].count;j++){
            FOOD* curr=&food[order[i].foodIndex[j]];
            if(order[i].in<=curr->captime) curr->captime+=curr->time;
            else curr->captime=order[i].in+curr->time;
        }
        return;
    }
    int out=0;
    for(int j=0;j<order[i].count;j++){
        FOOD* curr=&food[order[i].foodIndex[j]];
        int n=curr->captime-(curr->cap-1)*curr->time; //n is the first one's completed second.
        if(n>=order[i].in&&out<n) out=n;
        if(order[i].in<=curr->captime) curr->captime+=curr->time;
        else curr->captime=order[i].in+curr->time;
        order[i].out=out;
    }
    int afterCount=0;  //count orders that complete after current order.in, namely currently unfinished orders
    for(int j=0;j<i;j++) if(order[j].out>order[i].in) afterCount++;
    if(afterCount==w1){
        closeSec=order[i].in;
        openSec=w2thBigSec(i+1)+1;
    }
}

void singleOrderInputAndHandle(char* name, int curr_time){
    char _order[50];
    sprintf(_order, "%s %s\n", sec2time(curr_time), name);
    _singleOrderRead(_order);
    printf("%s", order[orderCount].name);
    ithOrderHandle(orderCount-1);
}

void orderHandle(){
    for(int i=0;i<orderCount;i++) ithOrderHandle(i);
    return;
}

void orderOutput(FILE* output){
    for(int i=0;i<orderCount;i++){
        if(order[i].out!=-1) secPrint(order[i].out, output);
        else fprintf(output, "Fail\n");
    }
}
/*
void inputRead(FILE* input, FILE* output){
    fscanf(input, "%d", &orderCount);
    order=(ORDER*)malloc((orderCount)*sizeof(ORDER));
    for(int i=0;i<orderCount;i++) singleOrderRead(input);
    orderHandle();
    orderOutput(output);
}
 */
void fileOpen(){
    for(int i=0; i<MAX_COMBO; i++){
        memset(combo[i].name, 0, sizeof(combo[i].name));
        memset(combo[i].foodIndex, -1, sizeof(combo[i].foodIndex));
    }
    for(int i=0; i<MAX_FOOD; i++) memset(food[i].name, 0, sizeof(food[i].name));
    dict=fopen("dict.dic", "r");
    input=fopen("input.txt", "r");
    //output=fopen("output.txt", "w+");
    //input=stdin;
    output=stdout;
    dictRead(dict);
    //inputRead(input, output);
    int discard;
    fscanf(input, "%d\n", &discard);
}

void fileClose(){
    fclose(dict);
    fclose(input);
    fclose(output);
}