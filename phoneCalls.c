/*
* File: phoneCalls.c
* Author: Melissa McMahon
* Purpose: This program takes in phone numbers and checks to see if they are at all related to each other, in being
* that it checks if they have called each other or if they have called the same third party extension.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// int linkedListLength = 0;
// int wordCounter;
// int globalTokens = 0;
struct PNMain* headMain = NULL;
// struct PNCalls* headCalls = NULL;
struct PNMain* currentMain = NULL;
//struct PNCalls* currentCalls = NULL;
//this struct is for my main linkedList
struct PNMain {
	char* phoneNumber;
	struct PNMain* next;
	struct PNCalls* head;
};
//this struct is for my smaller linked list.
struct PNCalls {
	int callCount;
	struct PNCalls* next;
	char* phoneNumber;
};
//freeNodes goes through all of the linkedlists and frees each phoneNumber and each Node.
void freeNodes() {
	struct PNMain* temp = headMain;
	struct PNCalls* miniList = temp->head;
	struct PNMain* mainFree = headMain;
	struct PNCalls* miniFree = temp->head;

	while (temp != NULL) {
		while (miniList != NULL) {
			// go through the main list
			miniFree = miniList;
			miniList = miniList->next;
			// free the phoneNumber of the mini struct
			free(miniFree->phoneNumber);
			// the free the actual smaller struct
			free(miniFree);
		}
		mainFree = temp;
		temp = temp->next;
		// free the phoneNumber string from the main list
		free(mainFree->phoneNumber);
		// then free the whole node
		free(mainFree);
		// stop before you get a seg fault.
		if (temp == NULL) {
			return;
		}
		miniList = temp->head;
	}
}
//commonConnections checks for commonConnections between callers
void commonConnections(char* caller, char* recipient) {
	// variables to go through the lists.
	struct PNMain* callerMain = headMain;
	struct PNMain* callerMain2 = headMain;
	struct PNCalls* miniListHead1 = NULL;
	struct PNCalls* miniListHead2 = NULL;
	// compare the strings util you find the first number you were given in the main list.
	while (callerMain != NULL) {
		if (strcmp(caller, callerMain->phoneNumber) == 0) {
			// then set its smaller list to the variable up above.
			miniListHead1 = callerMain->head;
			break;
		}
		callerMain = callerMain->next;
	}
	while (callerMain2 != NULL) {
		// now look for the second number's main list.
		if (strcmp(recipient, callerMain2->phoneNumber) == 0) {
			//repeat as before, make its minilist head saved to the variable above.
			miniListHead2 = callerMain2->head;
			break;
		}
		callerMain2 = callerMain2->next;
	}
//because I did a dual insertion I just have to iterate through one list and compare its numbers to that of the 
// first smaller list and look for a match, if I find a match then print yes and return from this function.
	while (miniListHead1 != NULL) {
		while (miniListHead2 != NULL) {
			if (strcmp(miniListHead1->phoneNumber, miniListHead2->phoneNumber) == 0) {
				printf("yes\n");
				return;
			}
			miniListHead2 = miniListHead2->next;
		}
		miniListHead2 = callerMain2->head;
		miniListHead1 = miniListHead1->next;
	}
	printf("no\n");
	return;
}
//printMe prints out how many times two numbers have contacted each other
void printMe(char* caller, char* recipient) {
	struct PNMain* temp = headMain;
	struct PNCalls* miniList = temp->head;
// because I did my list with double insertion, I just have to find the first number in my main list, and then look for 
// the second number in its mini list, if I find it print out callCount!
	while (temp != NULL) {
		if (strcmp(temp->phoneNumber, caller) == 0) {
			while (miniList != NULL) {
				if (strcmp(recipient, miniList->phoneNumber) == 0) {
					printf("%d\n", miniList->callCount);
					return;
				}
				miniList = miniList->next;
			}
		}
		temp = temp->next;
		if (temp == NULL) {
			printf("0\n");
			return;
		}
		miniList = temp->head;
	}
}
// initializeNumberOne adds to the main Linked list, if the number isnt already there and returns a pointer to that Node
// to be used to insert the second number.
struct PNMain* initializeNumberOne(char* number1) {
	// create the struct pointer for the head main list.
	struct PNMain* temp = headMain;
	while (temp != NULL) {
		// if the number is already in the list, then just return the pointer to it.
		if (strcmp(number1, temp->phoneNumber) == 0) {
			return temp;
		}
		temp = temp->next;
	}
	// if its not in the list, then make the struct, insert it, and then return the pointer to it.
	struct PNMain* tempMain = (struct PNMain*)calloc(1, sizeof(struct PNMain));
	// checking if malloc failed
	if (tempMain == NULL) {
		fprintf(stderr, "malloc failed!\n");
		exit(1);
	}
	tempMain->phoneNumber = strdup(number1);
	if (headMain == NULL) {
		headMain = tempMain;
		currentMain = tempMain;
		return tempMain;
	}
	else {
		currentMain -> next = tempMain;
		currentMain = tempMain;
		return tempMain;
	}
}
//insertN2 inserts the second number into the linked list, after checking whether it is there or not, if it is there
// it increments callCount instead.
void insertN2(struct PNMain* mainList , char* number2) {
	//create a struct to be initialized
	struct PNCalls* tempPNCalls = calloc(1,sizeof(struct PNCalls));
	if (tempPNCalls == NULL) {
		fprintf(stderr, "malloc failed!\n");
		exit(1);
	}
	// if the list has nothing in it, then insert the struct as the head.
	if (mainList->head == NULL) {
		mainList->head = tempPNCalls;
		tempPNCalls->phoneNumber = strdup(number2);
		tempPNCalls->callCount = 1;
		return;
	}
	// otherwise, look through the list, if you find it then increase the call count and free that struct.
	struct PNCalls* walker = mainList->head;
	if (strcmp(walker->phoneNumber, number2) == 0) {
		walker->callCount += 1;
		free(tempPNCalls);
		return;
	}
	// otherwise the number was not in the list, so add it.
	while (walker->next != NULL) {
		walker = walker->next;
		if (strcmp(walker->phoneNumber, number2) == 0) {
			walker->callCount += 1;
			free(tempPNCalls);
			return;
		}
	}
	walker->next = tempPNCalls;
	tempPNCalls->phoneNumber = strdup(number2);
	tempPNCalls->callCount = 1;
}
// main does all my error checking for the input and calls insertnumber one and insertN2.
int main(int argc, char** args) {
	//creating a filepointer
	FILE *fp;
	//calloc'ing my two strings for the file input.
	char* number1 = calloc(9 , sizeof(char));
	if (number1 == NULL) {
		fprintf(stderr, "malloc failed!\n");
		exit(1);
	}
	char* number2 = calloc(9 , sizeof(char));
	if (number2 == NULL) {
		fprintf(stderr, "malloc failed!\n");
		exit(1);
	}
	//some variables to be used with flagging
	int emptyFile = 0;
	int wordsRead;
	int i = 0;
	int anyErrors = 0;
	int j = 0;
	int l = 0;
//if you read in a command line argument string that is above 8, exit program that number isnt elegible.
	if(strlen(args[argc-2]) > 8 || strlen(args[argc-1]) > 8){
				fprintf(stderr, "number not eleigible\n");
				exit(1);
			}

	 if (argc < 4)
	 	// if there are less than 3 command line arugments, then exit
	{
		fprintf(stderr, "You must give Me Arguments");
		exit(1);
	}
	for (l = 0; l < 8; l++) {
		// check if the numbers you got from the command line are in the correct format.
		if (l == 3) {
			if (args[argc - 2][l] != '-' || args[argc - 1][l] != '-') {
				fprintf(stderr, "Arguments must be numbers\n");
			}
				continue;
			}
		else if (isdigit(args[argc - 2][l]) == 0 || isdigit(args[argc - 1][l]) == 0) {
			fprintf(stderr, "Arguments must be numbers\n");
			exit(1);
		}
	}
//open and read the file, but skip the executable and skip the two numbers at the end.
	for (i = 1; i < argc - 2 ; i++) {
		fp = fopen(args[i], "r");
		//check to make sure the file could be opened.
		if (fp == NULL) {
			fprintf(stderr, "File could not be opened\n");
			anyErrors = 1;
			//continue;
		}
// read until EOF
		while ((wordsRead = fscanf(fp, "%8s %8s\n", number1, number2)) != EOF) {
			// check each number in the file and make sure they are in the correct format.
			for (j = 0; j < 8; j++) {
				if (j == 3) {
					if (number1[j] != '-' || number2[j] != '-') {
						fprintf(stderr, "Arguments must be numbers\n");
					}
						continue;
					}
				if (isdigit(number1[j]) == 0 || isdigit(number2[j]) == 0) {
					fprintf(stderr, "Arguments must be numbers\n");
					exit(1);
				}
			}
			// if the file was empty flad
			 if (wordsRead > 0) {
				emptyFile = 1;
			}
			//make sure the two phoneNumbers in the command line arent calling themselves, or in the file.
			if (strcmp(number2, number1) == 0) {
				fprintf(stderr, "A phone cannot call itself!\n");
				anyErrors = 1;
			}
			// double insert the numbers from the file.
			insertN2(initializeNumberOne(number1), number2);
			insertN2(initializeNumberOne(number2), number1);
		}
		fclose(fp);
	}
	// throw an error but keep going.
	if (emptyFile == 0) {
		fprintf(stderr, "ERROR: Empty File\n");
		anyErrors = 1;
	}
	printMe(args[argc - 2], args[argc - 1]);
	commonConnections(args[argc - 2], args[argc - 1]);
	freeNodes();
	free(number2);
	free(number1);
	return anyErrors;
}