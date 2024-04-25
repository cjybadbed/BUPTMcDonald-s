#include <stdio.h>
#include <stdlib.h>

void secPrint(int n){
    int hour=7+(n-n%3600)/3600;
    n%=3600;
    int min=(n-n%60)/60;
    int sec=n%60;
    printf("%d:%d:%d",hour,min,sec);
}

int time2sec(char time[9]){
    int hour=(time[0]-'0')*10+time[1]-'0'-7,
        min=(time[3]-'0')*10+time[4]-'0',
        sec=(time[6]-'0')*10+time[7]-'0';
    int n=hour*3600+min*60+sec;
    return n;
}

int main(int argc, char** argv){

}