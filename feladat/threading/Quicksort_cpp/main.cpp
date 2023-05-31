#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <cstdlib>
#include <omp.h>

void fillArr(std::vector<int>& arr, int len){
    for(int i = 0; i < len; i++){
        arr.push_back(std::rand());
    }
}

template <typename T>
int partitionReq(std::vector<T>& arr, int left, int right) {
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
void quicksortReq(std::vector<T>& arr, int left, int right) {
    if (left < right) {
        int pivotIndex = partitionReq(arr, left, right);
        quicksortReq(arr, left, pivotIndex - 1);
        quicksortReq(arr, pivotIndex + 1, right);
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

        std::thread leftThread(quicksort<T>, std::ref(arr), left, pivotIndex - 1);
        std::thread rightThread(quicksort<T>, std::ref(arr), pivotIndex + 1, right);

        leftThread.join();
        rightThread.join();
    }
}

int main() {
    std::srand(std::time(nullptr));
    std::vector<int> array, array2; 
    fillArr(array, 100000);
    array2 = array;

    int size = array.size();
    double start; 
    double end; 
    start = omp_get_wtime();
    quicksort(array, 0, size - 1);
    end = omp_get_wtime(); 
    printf("Parallel execution time: %lf seconds\n", end - start);

    size = array2.size();
    start = omp_get_wtime();
    quicksortReq(array2, 0, size - 1);
    end = omp_get_wtime(); 
    printf("Reqential execution time: %lf seconds\n", end - start);

    return 0;
}