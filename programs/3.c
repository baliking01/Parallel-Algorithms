#include <stdio.h>

int main(){

    int begin, end, ok;
    do{
        ok = 1;
        printf("Interval bounds: \n");
        if(scanf("%d %d", &begin, &end) != 2){
            printf("Error! Invalid arguments\n");
            ok = 0;
        }
        if(begin >= end){
            printf("Error! Invalid bounds!\n");
            ok = 0;
        }
    }while(!ok);
    
    return 0;
}