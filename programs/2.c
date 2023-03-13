#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void printRandom(int lower, int upper){
    printf("%d\n", (rand() %(upper - lower + 1)) + lower);
}

void printRandomf(int lower, int upper){
    printf("%lf\n", ( (double)rand() * ( upper - lower ) ) / (double)RAND_MAX + lower);
}

int main(){

    srand(time(0));
    printRandom(500, 1000);
    printRandomf(500, 1000);
    return 0;
}
