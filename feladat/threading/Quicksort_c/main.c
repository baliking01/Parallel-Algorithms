#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_SIZE 100000

typedef struct {
    int* array;
    int left;
    int right;
} ThreadArgs;

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int* array, int left, int right) {
    int pivot = array[right];
    int i = left - 1;
    
    for (int j = left; j <= right - 1; j++) {
        if (array[j] <= pivot) {
            i++;
            swap(&array[i], &array[j]);
        }
    }
    
    swap(&array[i + 1], &array[right]);
    return i + 1;
}

void* quicksort(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int* array = args->array;
    int left = args->left;
    int right = args->right;
    
    if (left < right) {
        int pivot = partition(array, left, right);
        
        pthread_t thread1, thread2;
        
        ThreadArgs args1 = { array, left, pivot - 1 };
        ThreadArgs args2 = { array, pivot + 1, right };
        
        pthread_create(&thread1, NULL, quicksort, (void*)&args1);
        pthread_create(&thread2, NULL, quicksort, (void*)&args2);
        
        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
    }
    
    pthread_exit(NULL);
}

void fillArr(int* arr, int size){
    for(int i = 0; i < size; i++){
        arr[i] = rand() % 10000;
    }
}

int main() {
    int array[MAX_SIZE];
    int size;
    
    printf("Enter the number of elements (max %d): ", MAX_SIZE);
    scanf("%d", &size);
    
    if (size > MAX_SIZE) {
        printf("Size exceeds maximum limit\n");
        return 1;
    }
    
    fillArr(array, size);
    
    ThreadArgs args = { array, 0, size - 1 };
    
    pthread_t mainThread;
    pthread_create(&mainThread, NULL, quicksort, (void*)&args);
    pthread_join(mainThread, NULL);
    
    printf("Sorted array:\n");
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
    
    return 0;
}