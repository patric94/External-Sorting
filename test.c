#include "sorted.h"
#include "BF.h"
//#define fileName "heapFile"
#define MAX_RECS BLOCK_SIZE/sizeof(Record)


int insert_Entries(int fd) {
	FILE *stream;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    stream = stdin;
    Record record;
	int blockToInsert = 0, totalRecs = 0;
	unsigned int offset;
	void *block;

    while ((read = getline(&line, &len, stream)) != -1) {
        line[read - 2] = 0;
        char *pch;

        pch = strtok(line, ",");
        record.id = atoi(pch);

        pch = strtok(NULL, ",");
        pch++;
        pch[strlen(pch) - 1] = 0;
        strncpy(record.name, pch, sizeof(record.name));

        pch = strtok(NULL, ",");
        pch++;
        pch[strlen(pch) - 1] = 0;
        strncpy(record.surname, pch, sizeof(record.surname));

        pch = strtok(NULL, ",");
        pch++;
        pch[strlen(pch) - 1] = 0;
        strncpy(record.city, pch, sizeof(record.city));

		if ((totalRecs % (MAX_RECS)) == 0) {
			if (totalRecs) { 		//Grafoume to proigoumeno block pou gemise.
				if(BF_WriteBlock(fd , blockToInsert) < 0){
					BF_PrintError("Error writing block");
					return -1;
				}
			}

			if (BF_AllocateBlock(fd) < 0) {
				BF_PrintError("Error allocating block");
				return -1;
			}
			blockToInsert++;
			if(BF_ReadBlock(fd, blockToInsert, &block) < 0){
				BF_PrintError("Error reading block");
				return -1;
			}
		}

		offset = (totalRecs % (MAX_RECS))*sizeof(Record);
		memcpy(block + offset, &record, sizeof(Record));

		totalRecs++;
    }
    free(line);
	return totalRecs;
}


int main(int argc, char **argv) {

    int heap_fd, sorted_fd;

    BF_Init();

	char *fileName;
	fileName = (char*)malloc(strlen("heapFile") + 1);
	strcpy(fileName, "heapFile");

    //create heap file
    if(Sorted_CreateFile(fileName) == -1 ){
        printf("Error creating file!\n");
		return EXIT_FAILURE;
	}


    if( (heap_fd = Sorted_OpenFile(fileName)) < 0  ){
        printf("Error opening file!\n");
		return EXIT_FAILURE;
	}
	int a;
	if ((a = insert_Entries(heap_fd)) == -1) {
		printf("Error inserting record at heap file\n");
		return EXIT_FAILURE;
	}

	// printf("SIZEOFREC = %lu\n",sizeof(Record) );
	// printf("BLOCKS = %d\n",BF_GetBlockCounter(heap_fd));
	// printf("TOTALRECS = %d\n",a);

	if (Sorted_CloseFile(heap_fd) == -1) {
		printf("Error closing file!\n");
		return EXIT_FAILURE;
	}

    //sort heap file using 2-way merge-sort
	int fieldNo = 0;
	char *finalName;
	finalName = (char*) malloc(strlen(fileName) + strlen("Sorted") + 2);
	sprintf(finalName, "%sSorted%d", fileName, fieldNo);

    if(Sorted_SortFile(fileName,fieldNo) == -1  ){
        printf("Error sorting file!\n");
		return EXIT_FAILURE;
	}

	/*---------------------------------------------------------------*/
	if( (sorted_fd = Sorted_OpenFile(finalName)) < 0  ){
        printf("Error opening file!\n");
		return EXIT_FAILURE;
	}

	//get all entries with value
	//Sorted_GetAllEntries(sorted_fd, &fieldNo, NULL); //&value);

    int value = 11903588;
	// char value[20];
    // strcpy(value, "Keratsini");
	if(Sorted_checkSortedFile(finalName, fieldNo) == -1){
		printf("Error ! File is not sorted!\n");
	}
	Sorted_GetAllEntries(sorted_fd, &fieldNo, &value);

	if(Sorted_CloseFile(sorted_fd) < 0){
		BF_PrintError("Error closing file");
		return -1;
	}
	/*---------------------------------------------------------------*/


	free(finalName);
	free(fileName);
    return EXIT_SUCCESS;
}
