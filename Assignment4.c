// Jarod Cavagnaro
// CSC 139 Section 2
// 12/5/2022
// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *Out;

// Data Check Function
void checkValues(int numPages, int numFrames, int numRequests, int reqArray[20]);

// Memory Management Functions
void FIFO(int numPages, int numFrames, int numRequests, int reqArray[20]);
void Optimal(int numPages, int numFrames, int numRequests, int reqArray[20]);
void LRU(int numPages, int numFrames, int numRequests, int reqArray[20]);

// Queue Functions
void newQueue(int queue[], int size);
int checkForPage(int queue[], int page);
int findNextEmpty(int queue[]);
int findOptimal(int queue[], int sizeQueue, int reqArray[], int sizeArray, int index);
int findLeastUsed(int queue[], int timer[], int size);

int main() {
	
	FILE *In = fopen("test5.txt", "r");
	
	Out = fopen("output.txt", "w");
	
	printf("Input:\n");
	
	// Check that the file is not empty
    if(In == NULL) {
        perror("Empty File");
        exit(1);
    }
	
// Input
	int numPages;
	int numFrames;
	int numRequests;
	int reqArray[20];
	
	char buff_in[20];
	int line = 0;
	char val[10];
	
	while(fgets(buff_in, sizeof(buff_in), In) != NULL) {
		if(line == 0) { // read first line
			int lastIndex = 0;
			int dataCount = 0;
			
			printf("%s", buff_in);
			fprintf(Out, "%s", buff_in); // print to output file
			
			// Set the starting page/frame/request counts
			for(int i = 0; i < strlen(buff_in); i++) {
				if(buff_in[i] != '	') {
					strncpy(val, buff_in + i, 2);
					if(dataCount == 0) {
						numPages = atoi(val);
					} else if (dataCount == 1) {
						numFrames = atoi(val);
					} else if(dataCount == 2) {
						numRequests = atoi(val);
					}
					
					dataCount++;
					if(dataCount > 2) {
						break;
					}
				}
			}
			line++;
		} else { // read all other lines
			printf("%s", buff_in);
			fprintf(Out, "%s", buff_in); // print to output file
			strcpy(val, buff_in);
			reqArray[line-1] = atoi(&buff_in[0]); // add page requests to an array
			line++;
		}
	}
	printf("\n");
	fprintf(Out, "\n");
	
	// Check Values
	//checkValues(numPages, numFrames, numRequests, reqArray);
	
// Output
	printf("Output:\n");
	fprintf(Out, "Output:\n");
	
	// Call Management Functions
	FIFO(numPages, numFrames, numRequests, reqArray);
	printf("\n");
	fprintf(Out, "\n");
	Optimal(numPages, numFrames, numRequests, reqArray);
	printf("\n");
	fprintf(Out, "\n");
	LRU(numPages, numFrames, numRequests, reqArray);
}

// Create zeroed out queue
void newQueue(int queue[], int size) {
	for(int i = 0; i < size; i++) {
		queue[i] = 0;
	}
}

// Check queue for existing pages
int checkForPage(int queue[], int page) {
	for(int i = 0; i < sizeof(queue); i++) {
		if(queue[i] == page) {
			return i;
		}
	}
	return -1;
}

// returns the index of the next empty space - returns -1 if none
int findNextEmpty(int queue[]) {
	for(int i = 0; i < sizeof(queue); i++) {
		if(queue[i] == 0) {
			return i;
		}
	}
	return -1;
}

// check for page with longest wait - returns optimal index to switch
int findOptimal(int queue[], int sizeQueue, int reqArray[], int sizeArray, int index) {
	int counter[sizeQueue];
	newQueue(counter, sizeQueue);
	
	// from index to the end of the request list
	for(int i = index + 1; i < sizeArray; i++) {
		// compare every value in queue to every value in reqArray
		for(int j = 0; j < sizeQueue; j++) {
			// if they are equal add 1 to counter at index j
			if(reqArray[i] == queue[j]) {
				counter[j] += 1;
			}
		}
	}
	
	// find smallest
	int smallest = 0;
	for(int i = 0; i < sizeQueue; i++) {
		if(counter[i] < counter[smallest]) {
			smallest = i;
		}
	}
	return smallest;
}

// return the index of the least recently referenced page
int findLeastUsed(int queue[], int timer[], int size) {
	int longest = 0;
	for(int i = 0; i < size; i++) {
		if(timer[i] > timer[longest]) {
			longest = i;
		}
	}
	return longest;
}

// First-In-First-Out
void FIFO(int numPages, int numFrames, int numRequests, int reqArray[20]) {
	printf("FIFO\n");
	fprintf(Out, "FIFO\n");
	
	// Queue to store pages in frames
	int memory[numFrames];
	newQueue(memory, numFrames);
	
	int pageFaults = 0;
	int nextIndex = 0;
	
	// loop through requests
	for(int i = 0; i < numRequests; i++) {
		int usedFrame = checkForPage(memory, reqArray[i]);
		if(usedFrame == -1) {
			if(memory[nextIndex] == 0) {
				memory[nextIndex] = reqArray[i];
			} else {
				printf("Page %d unloaded from Frame %d, ", memory[nextIndex], nextIndex);
				fprintf(Out, "Page %d unloaded from Frame %d, ", memory[nextIndex], nextIndex);
				memory[nextIndex] = reqArray[i];
			}
			printf("Page %d loaded into Frame %d\n", reqArray[i], nextIndex);
			fprintf(Out, "Page %d loaded into Frame %d\n", reqArray[i], nextIndex);
			nextIndex++;
			if(nextIndex >= numFrames) nextIndex = 0;
			pageFaults++;
		} else {
			printf("Page %d already in Frame %d\n", reqArray[i], usedFrame);
			fprintf(Out, "Page %d already in Frame %d\n", reqArray[i], usedFrame);
		}
	}
	
	printf("%d page faults\n", pageFaults);
	fprintf(Out, "%d page faults\n", pageFaults);
}

void Optimal(int numPages, int numFrames, int numRequests, int reqArray[20]) {
	printf("Optimal\n");
	fprintf(Out, "Optimal\n");
	
	int pageFaults = 0;
	
	// Queue to store pages in frames
	int memory[numFrames];
	newQueue(memory, numFrames);
	
	for(int i = 0; i < numRequests; i++) {
		int usedFrame = checkForPage(memory, reqArray[i]);
		if(usedFrame == -1) { // if the current page requested is not already loaded
			int nextIndex = findNextEmpty(memory); // check for empty space - if none return -1
			if(nextIndex == -1) {
				nextIndex = findOptimal(memory, numFrames, reqArray, numRequests, i); // get the optimal next index
				printf("Page %d unloaded from Frame %d, ", memory[nextIndex], nextIndex);
				fprintf(Out, "Page %d unloaded from Frame %d, ", memory[nextIndex], nextIndex);
				memory[nextIndex] = reqArray[i];
			} else {
				memory[nextIndex] = reqArray[i];
			}
			printf("Page %d loaded into Frame %d\n", reqArray[i], nextIndex);
			fprintf(Out, "Page %d loaded into Frame %d\n", reqArray[i], nextIndex);
			pageFaults++;
		} else {
			printf("Page %d already in Frame %d\n", reqArray[i], usedFrame);
			fprintf(Out, "Page %d already in Frame %d\n", reqArray[i], usedFrame);
		}
	}
	
	printf("%d page faults\n", pageFaults);
	fprintf(Out, "%d page faults\n", pageFaults);
}

void LRU(int numPages, int numFrames, int numRequests, int reqArray[20]) {
	printf("LRU\n");
	fprintf(Out, "LRU\n");
	
	// Queue to store pages in frames
	int memory[numFrames];
	newQueue(memory, numFrames);
	
	int pageFaults = 0;
	
	int timer[numFrames];
	newQueue(timer, numFrames);
	int unload = 0;
	int nextIndex;
	
	for(int i = 0; i < numRequests; i++) {
		int usedFrame = checkForPage(memory, reqArray[i]);
		if(usedFrame == -1) { // if the current page requested is not already loaded
			nextIndex = findNextEmpty(memory); // check for empty space - if none return -1
			if(nextIndex == -1) {
				nextIndex = findLeastUsed(memory, timer, numFrames); // get the index that has not been accessed the longest
				printf("Page %d unloaded from Frame %d, ", memory[nextIndex], nextIndex);
				fprintf(Out, "Page %d unloaded from Frame %d, ", memory[nextIndex], nextIndex);
				memory[nextIndex] = reqArray[i];
				unload = 1;
			} else {
				memory[nextIndex] = reqArray[i];
				unload = 0;
			}
			printf("Page %d loaded into Frame %d\n", reqArray[i], nextIndex);
			fprintf(Out, "Page %d loaded into Frame %d\n", reqArray[i], nextIndex);
			pageFaults++;
		} else {
			printf("Page %d already in Frame %d\n", reqArray[i], usedFrame);
			fprintf(Out, "Page %d already in Frame %d\n", reqArray[i], usedFrame);
			nextIndex = usedFrame;
			unload = 1;
		}
		
		// increment timer
		for(int i = 0; i < numFrames; i++) {
			if(memory[i] != 0) {
				if(unload == 0 || (unload == 1 && i != nextIndex)) {
					timer[i] += 1;
				} else {
					timer[i] = 0;
				}
			}
		}
	}
	
	printf("%d page faults\n", pageFaults);
	fprintf(Out, "%d page faults\n", pageFaults);
}

void checkValues(int numPages, int numFrames, int numRequests, int reqArray[20]) {
	printf("NumPages: %d\n", numPages);
	printf("NumFrames: %d\n", numFrames);
	printf("NumRequests: %d\n", numRequests);
	printf("Requests: ");
	for(int i = 0; i < numRequests; i++) {
		printf("%d, ", reqArray[i]);
	}
}