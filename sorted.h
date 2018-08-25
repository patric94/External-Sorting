#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <unistd.h>



typedef struct{
	int id;
	char name[15];
	char surname[20];
	char city[25];
}Record;


typedef struct{
	char type;
	int fieldNo;
	int totalRecs;
	int mode;
}S_Info;


int Sorted_CreateFile(char *fileName);
int Sorted_OpenFile(char *fileName);
int Sorted_CloseFile(int fileDesc);
int Sorted_InsertEntry(int fileDesc, Record record);
int Sorted_SortFile(char *fileName, int fieldNo);
int Sorted_checkSortedFile(char *fileName, int fieldNo);
int Sorted_GetAllEntries(int fileDesc, int *fieldNo, void *value);
int recordCompare(Record *a, Record *b, int fieldNo, int mode);
int createTempFile(int number, int pass);
void BubbleSort(Record **recArray, int arraySize,int fieldNo, int mode);
void printRec(Record *rec);
