#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

int main(int argc, char* args[]){
	
	if(argc <= 1){
		printf("No arguments given!\n"
		"Expected arguments for program:\n"
		"\t-path\n"
		"\t-element count\n");
		return 0;
	}
	else if(argc == 2){
		printf("Element count argument missing!\n"
		"Expected arguments for program:\n"
		"\t-path\n"
		"\t-element count\n");
		return 0;
	}
	
	int elementCount;
	if(isdigit(*args[2])){
		elementCount = atoi(args[2]);
	} 
	else{
		printf("Argument '%s' is not a valid number!", args[2]);
		return 0;
	}
	
	clock_t start, end, startProg, endProg;
	double writeTime;
	double saveTime;
	srand(time(NULL));
	
	// For testing set an upper bound for file size
	const int maxCount = 20;
	
	startProg = clock();
	FILE* fp = fopen(strcat(args[1], ".txt"), "w+");
	if(fp != NULL) printf("File opened!\n");
	
	// Measure time taken by for loop
	int size = 10000;
	
	for(int i = 0; i < maxCount; i++) {
		start = clock();
		for(int j = 0; j < i * size; j++){
			fprintf(fp, "%d\n", rand());
		}
		end = clock();
		writeTime = ((double) (end - start)) / CLOCKS_PER_SEC;
		printf("Written %d elements in %lf sec time\n", i*size, writeTime);
		size += 4000;
	}
	
	printf("Written %d times to %s\n"
	"Closing file...\n", elementCount, args[1]);
	
	// Measure time taken for saving the file
	fclose(fp);
	endProg = clock();
	saveTime = ((double) (endProg - startProg)) / CLOCKS_PER_SEC;
	
    printf("Total time taken to finish operation on file: %lf sec", saveTime);
	
	return 0;
}
