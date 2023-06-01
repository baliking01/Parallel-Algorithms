#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <cstdlib>
#include <stack>
#include <omp.h>

unsigned int max_threads = 1;
unsigned int current_threads = 0;
unsigned int min_elements = 1;

void fillArr(std::vector<int>& arr, int len){
    arr.clear();
    for(int i = 0; i < len; i++){
        arr.push_back(std::rand());
    }
}

template <typename T>
int partition(std::vector<T>& arr, int left, int right) {
    T pivot = arr[right];
    int i = left - 1;

    for (int j = left; j < right; ++j) {
        if (arr[j] <= pivot) {
            ++i;
            std::swap(arr[i], arr[j]);
        }
    }

    std::swap(arr[i + 1], arr[right]);
    return i + 1;
}

template <typename T>
void quicksort(std::vector<T>& arr, int left, int right) {
    if (left < right) {
        int pivotIndex = partition(arr, left, right);
        quicksort(arr, left, pivotIndex - 1);
        quicksort(arr, pivotIndex + 1, right);
    }
}

template <typename T>
void parallelQuicksort(std::vector<T>& arr, int left, int right) {
    if(right - left < min_elements){
        quicksort(arr, left, right);
        return;
    }

    if (left < right) {
        int pivotIndex = partition(arr, left, right);

        std::thread leftThread(parallelQuicksort<T>, std::ref(arr), left, pivotIndex - 1);
        std::thread rightThread(parallelQuicksort<T>, std::ref(arr), pivotIndex + 1, right);

        leftThread.join();
        rightThread.join();
    }
}

template <typename T>
void iterativeQuicksort(std::vector<T>& arr) {
    int size = arr.size();

    std::stack<std::pair<int, int>> stack;
    stack.push(std::make_pair(0, size - 1));

    while (!stack.empty()) {
        int left = stack.top().first;
        int right = stack.top().second;
        stack.pop();

        int pivotIndex = partition(arr, left, right);

        if (pivotIndex - 1 > left) {
            stack.push(std::make_pair(left, pivotIndex - 1));
        }
        if (pivotIndex + 1 < right) {
            stack.push(std::make_pair(pivotIndex + 1, right));
        }
    }
}

void benchMark(unsigned int max_elements){
    if(max_elements < 100){
        printf("Maximum vector size smaller than starting vector size! This value should be at least 100!\n");
        return;
    }
    std::vector<int> vec1, vec2, vec3;
    double start;
    double end;

    FILE* fp1 = fopen("Quicksort.txt", "w");
    FILE* fp2 = fopen("ParallelQuicksort.txt", "w");
    FILE* fp3 = fopen("IterativeQuicksort.txt", "w");
    FILE* fp4 = fopen("NumberOfElements.txt", "w");

    for(unsigned int i = 100; i < max_elements; i+=10000){
        fillArr(vec1, i);
        vec2 = vec1;
        vec3 = vec1;

        start = omp_get_wtime();
        quicksort(vec1, 0, vec1.size() - 1);
        end = omp_get_wtime();
        fprintf(fp1, "%lf\n", end - start);

        start = omp_get_wtime();
        parallelQuicksort(vec2, 0, vec2.size() - 1);
        end = omp_get_wtime(); 
        fprintf(fp2, "%lf\n", end - start);

        start = omp_get_wtime();
        iterativeQuicksort(vec3);
        end = omp_get_wtime(); 
        fprintf(fp3, "%lf\n", end - start);
        fprintf(fp4, "%u\n", i);
    }

    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    fclose(fp4);
}

int main() {
    std::srand(std::time(nullptr));
    //benchMark(1000000);
    
    std::vector<int> array, array2, array3, array4; 
    fillArr(array, 100000);
    array2 = array;
    array3 = array;
    array4 = array;

    int size = array.size();
    double start; 
    double end; 

    start = omp_get_wtime();
    quicksort(array, 0, size - 1);
    end = omp_get_wtime(); 
    printf("Recursive execution time: %lf seconds\n", end - start);

    start = omp_get_wtime();
    parallelQuicksort(array2, 0, size - 1);
    end = omp_get_wtime(); 
    printf("Parallel execution time: %lf seconds\n", end - start);

    start = omp_get_wtime();
    iterativeQuicksort(array3);
    end = omp_get_wtime(); 
    printf("Iterative execu tion time: %lf seconds\n", end - start);

    FILE* fp = fopen("ParallelThreadLimit.txt", "w");

    while(min_elements <= 10000){
        start = omp_get_wtime();
        parallelQuicksort(array4, 0, size - 1);
        end = omp_get_wtime(); 
        fprintf(fp, "Parallel execution time with controlled split: %lf seconds\n", end - start);
        fprintf(fp, "%lf\n", end - start);
        fillArr(array4, 100000);
        min_elements += 100;
    }

    return 0;
}