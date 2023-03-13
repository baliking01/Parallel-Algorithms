#include <stdio.h>
#include <unistd.h>

int main(){
    for(int i = 0; i < 10; i++){
        printf("%d. iteration\n", i);
        printf("Waiting...\n");
        sleep(3);
    }
    return 0;
}