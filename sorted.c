#include "sorted.h"
#include "BF.h"
#define MAX_RECS BLOCK_SIZE/sizeof(Record)

int mergedRecs;

int recordCompare(Record *a, Record *b, int fieldNo, int mode){		//mode dhlwnei auksousa h' fthinousa sugkrish ( 1 h' 2 antistoixa )
	if (fieldNo == 0) {
		if (a->id > b->id) {
			if (mode == 1) {
				return 1;
			}
			else if (mode == 2){
				return -1;
			}
		}
		else{
			if (mode == 1) {
				return -1;
			}
			else if (mode == 2){
				return 1;
			}
		}
	}
	else if (fieldNo == 1) {
		if (strcmp(a->name, b->name) > 0) {
			if (mode == 1) {
				return 1;
			}
			else if (mode == 2) {
				return -1;
			}
		}
		else if (strcmp(a->name, b->name) < 0) {
			if (mode == 1) {
				return -1;
			}
			else if (mode == 2){
				return 1;
			}
		}
		else{
			if (a->id > b->id) {
				if (mode == 1) {
					return 1;
				}
				else if (mode == 2){
					return -1;
				}
			}
			else{
				if (mode == 1) {
					return -1;
				}
				else if (mode == 2){
					return 1;
				}
			}
		}
	}
	else if (fieldNo == 2) {
		if (strcmp(a->surname, b->surname) > 0) {
			if (mode == 1) {
				return 1;
			}
			else if (mode == 2) {
				return -1;
			}
		}
		else if (strcmp(a->surname, b->surname) < 0) {
			if (mode == 1) {
				return -1;
			}
			else if (mode == 2){
				return 1;
			}
		}
		else{
			if (a->id > b->id) {
				if (mode == 1) {
					return 1;
				}
				else if (mode == 2){
					return -1;
				}
			}
			else{
				if (mode == 1) {
					return -1;
				}
				else if (mode == 2){
					return 1;
				}
			}
		}
	}
	else if (fieldNo == 3) {
		if (strcmp(a->city, b->city) > 0) {
			if (mode == 1) {
				return 1;
			}
			else if (mode == 2) {
				return -1;
			}
		}
		else if (strcmp(a->city, b->city) < 0) {
			if (mode == 1) {
				return -1;
			}
			else if (mode == 2){
				return 1;
			}
		}
		else{
			if (a->id > b->id) {
				if (mode == 1) {
					return 1;
				}
				else if (mode == 2){
					return -1;
				}
			}
			else{
				if (mode == 1) {
					return -1;
				}
				else if (mode == 2){
					return 1;
				}
			}
		}
	}
	return 0;
}

int createTempFile(int number, int pass){
	int fd, numDigits, passDigits;
	char *newName;

	numDigits = floor(log10(abs(number))) + 1;
	passDigits = floor(log10(abs(pass))) + 1;

	if((newName = (char*) malloc((strlen("temp") + numDigits + strlen("_")+ passDigits + 1)*sizeof(char))) == NULL){
		perror("Temp name malloc error");
		return -1;
	}
	sprintf(newName, "temp%d_%d", number,pass);

	if( BF_CreateFile(newName) < 0){
		BF_PrintError("Error creating temp file");
		return -1;
    }

	if((fd = BF_OpenFile(newName)) < 0){
		BF_PrintError("Error opening temp file");
		return -1;
    }

	for (int i = 0; i < (int)pow(2, (double)(pass-1)); i++) {
		if(BF_AllocateBlock(fd) < 0){
			BF_PrintError("Error allocating block of temp file");
			return -1;
		}
	}
	free(newName);
	return fd;
}

void BubbleSort(Record **recArray, int arraySize,int fieldNo, int mode){

	int swapped;
	for (int i = 1; i < arraySize; i++) {
		swapped = 0;
		for (int j = 0; j < arraySize - i ; j++) {
			if (recordCompare(recArray[j], recArray[j+1], fieldNo, mode) == 1) {
				Record *temp = recArray[j+1];
				recArray[j+1] = recArray[j];
				recArray[j] = temp;
				swapped = 1;
			}
		}
		if (!swapped) {
			break;
		}
	}
}

int Sorted_CreateFile(char* fileName){
	int fd;
	void *block;
	S_Info info;

	if( BF_CreateFile(fileName) < 0){
		BF_PrintError("Error creating file");
		return -1;
    }

	if((fd = BF_OpenFile(fileName)) < 0){
		BF_PrintError("Error opening file");
		return -1;
    }

    if(BF_AllocateBlock(fd) < 0){
		BF_PrintError("Error allocating block");
		return -1;
    }

	info.type = 'S';
	info.fieldNo = -1;
	info.totalRecs = 0;

	if(BF_ReadBlock(fd, 0, &block) < 0){
    	BF_PrintError("Error reading block");
		return -1;
    }

	memcpy(block, &info, sizeof(S_Info));
	if(BF_WriteBlock(fd , 0) < 0){
    	BF_PrintError("Error writing block");
		return -1;
    }

	if(BF_CloseFile(fd) < 0){
    	BF_PrintError("Error closing file");
		return -1;
    }

	return 0;
}


int Sorted_OpenFile(char* fileName){
	int fd;
	void *block;
	S_Info info;

	if((fd = BF_OpenFile(fileName)) < 0){
		BF_PrintError("Error opening file");
		return -1;
    }

	if(BF_ReadBlock(fd, 0, &block) < 0){
    	BF_PrintError("Error reading block");
		return -1;
    }

	memcpy(&info, block, sizeof(S_Info));
	if (info.type != 'S') {
		return -2;
	}

	return fd;
}


int Sorted_CloseFile(int fileDesc){
	void *block;
	S_Info info;

	if(BF_ReadBlock(fileDesc, 0, &block) < 0){
    	BF_PrintError("Error reading block");
		return -1;
    }

	memcpy(&info, block, sizeof(S_Info));
	if (info.type != 'S') {
		return -1;
	}

	if(BF_CloseFile(fileDesc) < 0){
    	BF_PrintError("Error closing file");
		return -1;
    }

	return 0;
}


int Sorted_InsertEntry(int fileDesc, Record record){
	void *block;

	if(BF_ReadBlock(fileDesc, mergedRecs/(MAX_RECS) , &block) < 0){
		BF_PrintError("Error reading block");
		return -1;
	}
	memcpy(block + (mergedRecs % (MAX_RECS))*sizeof(Record), &record, sizeof(Record));
	if(BF_WriteBlock(fileDesc , mergedRecs/(MAX_RECS)) < 0){
    	BF_PrintError("Error writing block");
		return -1;
    }
	return 0;
}


int Sorted_SortFile(char *fileName, int fieldNo){
	int heap_fd, prevFiles, totalFiles, heapBlocks, pass = 1, temp_fd, final_fd, mode = 1;
	char *newName;
	void *heap_block, *temp_block, *final_block;
	S_Info info;

	if ((heap_fd = Sorted_OpenFile(fileName)) < 0) {
		if (heap_fd == -2) {
			printf("File type not \"sorted\"\n");
		}
		return -1;
	}

	if((newName = (char*) malloc((strlen(fileName) + strlen("Sorted") + 2)*sizeof(char))) == NULL){
		perror("New name malloc error");
		return -1;
	}
	sprintf(newName, "%sSorted%d", fileName,fieldNo);

	if ((heapBlocks = totalFiles = BF_GetBlockCounter(heap_fd) - 1) < 0) {
		BF_PrintError("Get block counter error");
		return -1;
	}

	do{
		//for (int i = 1; i <= totalFiles ; i++) {		//AUTO EDW DE DOULEUEI!!!
		for (int i = totalFiles; i >= 1 ; i--) {   //trexei apo to teleutaio pros to proto.
			if ((temp_fd = createTempFile(i, pass)) == -1) {
				return -1;
			}

			int lastBlockRecs = MAX_RECS;
			if (pass == 1) {			//sto prwto pass eswterikh taksinomhsh afoy ginei antigrafh dedomenwn apo to heap file
				Record **recArray = NULL;

				if(BF_ReadBlock(heap_fd, i, &heap_block) < 0){
			    	BF_PrintError("Error reading heap block");
					return -1;
			    }

				if(BF_ReadBlock(temp_fd, 0, &temp_block) < 0){
			    	BF_PrintError("Error reading temp block");
					return -1;
			    }

				recArray = malloc((MAX_RECS)*sizeof(Record*));
				for (int j = 0; j < (MAX_RECS) ; j++) {
					recArray[j] = malloc(sizeof(Record));
					memcpy(recArray[j] , heap_block + j*sizeof(Record), sizeof(Record));
					if (recArray[j]->id == 0) {		//an record.id == 0 tote diabasame sizeof(Record) bytes poy einai 0,dhladh to block einai misogemato. auto mporei na sumbei mono sto teleftaio block toy heapfile
						free(recArray[j]);
						recArray = realloc(recArray, j*sizeof(Record*));
						lastBlockRecs = j;
						break;
					}
				}

				BubbleSort(recArray, lastBlockRecs, fieldNo, mode);

				for (int j = 0; j < lastBlockRecs; j++) {
					memcpy(temp_block + j*sizeof(Record), recArray[j], sizeof(Record));
				}

				if(BF_WriteBlock(temp_fd , 0) < 0){
			    	BF_PrintError("Error writing temp block");
					return -1;
			    }

				for (int k = 0; k < lastBlockRecs; k++) {
					free(recArray[k]);
				}
				free(recArray);
			}
			else{
				char *tempName1, *tempName2;
				int numDigits1, numDigits2, passDigits, temp_fd1, temp_fd2;
				void *temp_block1, *temp_block2;

				numDigits1 = floor(log10(abs(prevFiles))) + 1;
				passDigits = floor(log10(abs(pass))) + 1;

				if((tempName1 = (char*) malloc((strlen("temp") + numDigits1 + strlen("_") + passDigits + 1)*sizeof(char))) == NULL){
					perror("Temp name1 malloc error");
					return -1;
				}
				sprintf(tempName1, "temp%d_%d", prevFiles,pass-1);
				if((temp_fd1 = BF_OpenFile(tempName1)) < 0){
					BF_PrintError("Error opening file");
					return -1;
				}

				if (prevFiles != 1) {
					numDigits2 = floor(log10(abs(prevFiles - 1))) + 1;
					if((tempName2 = (char*) malloc((strlen("temp") + numDigits2 + strlen("_") + passDigits + 1)*sizeof(char))) == NULL){
						perror("Temp name2 malloc error");
						return -1;
					}
					sprintf(tempName2, "temp%d_%d", prevFiles - 1,pass-1);

					if((temp_fd2 = BF_OpenFile(tempName2)) < 0){
						BF_PrintError("Error opening file");
						return -1;
					}
				}

				int k = 0, l = 0, flagK = 0, k_Read = 0, l_Read = 0;
				Record *a,*b;

				a = (Record*) malloc(sizeof(Record));
				b = (Record*) malloc(sizeof(Record));

				mergedRecs = 0;		//global gia na metraei tis eggrafes poy exoun mpei sto arxeio sugxwneushs

				while (( k < ((int)pow(2, (double)(pass-2))*(MAX_RECS)) )&&( l < ((int)pow(2, (double)(pass-2))*(MAX_RECS)) )&&( prevFiles != 1 )) {
					/*to sxoliasmeno kommati auto to kaname gia na diabazoume mia fora to block sto opoio briskomaste
						oso diabazoume eggrafes apo auto. Telika omws den douleuei swsta sto dataset 100.000 giati xanotan
						to periexomeno tou block autou meta apo kapoia memcpy (xwris na fainetai na kanoume peritta memcpy)*/

					// if (k % (MAX_RECS) == 0) {	//gia na mhn diabazw sunexeia to idio block
					// 	if(BF_ReadBlock(temp_fd1, k/(MAX_RECS) , &temp_block1) < 0){
					//     	BF_PrintError("Error reading block");
					// 		return -1;
					//     }
					// }
					if (k_Read == 0) {	//gia na mhn
						if(BF_ReadBlock(temp_fd1, k/(MAX_RECS) , &temp_block1) < 0){
					    	BF_PrintError("Error reading block");
							return -1;
					    }
						memcpy(a, temp_block1 + (k % (MAX_RECS))*sizeof(Record), sizeof(Record));
					}
					if (a->id == 0) {
						flagK = 1; //gia na brw meta oti teleiwse to arxeio k.
						break;
					}
					// if (l % (MAX_RECS) == 0) {	//gia na mhn diabazw sunexeia to idio block
					// 	if(BF_ReadBlock(temp_fd2, l/(MAX_RECS) , &temp_block2) < 0){
					//     	BF_PrintError("Error reading block");
					// 		return -1;
					//     }
					// }
					if (l_Read == 0) {
						if(BF_ReadBlock(temp_fd2, l/(MAX_RECS) , &temp_block2) < 0){
					    	BF_PrintError("Error reading block");
							return -1;
					    }
						memcpy(b, temp_block2 + (l % (MAX_RECS))*sizeof(Record), sizeof(Record));
					}
					if (b->id == 0) {
						 break;
					}
					if (recordCompare(a, b, fieldNo, mode) == 1) {
						if (Sorted_InsertEntry(temp_fd,*b) == -1) {
							return -1;
						}
						l++;
						k_Read++;
						l_Read = 0;
					}
					else{
						if (Sorted_InsertEntry(temp_fd,*a) == -1) {
							return -1;
						}
						k++;
						l_Read++;
						k_Read = 0;
					}
					mergedRecs++;
				}
				//printf("LALALA ME K = %d KAI L = %d KAI FLAGK = %d\n",k,l,flagK );
				//antigrafh upoloipomenwn eggrafwn
				if (k < ((int)pow(2, (double)(pass-2))*(MAX_RECS)) && !flagK) { 	// eite den exoume deutero arxeio, eite teleiwse to deutero
						while (k < ((int)pow(2, (double)(pass-2))*(MAX_RECS))) {	// ara antigrafoume oses emeinan apo to prwto.
							if (k % (MAX_RECS) == 0) {	//gia na mhn diabazw sunexeia to idio block
								if(BF_ReadBlock(temp_fd1, k/(MAX_RECS) , &temp_block1) < 0){
							    	BF_PrintError("Error reading block");
									return -1;
							    }
							}
							memcpy(a, temp_block1 + (k % (MAX_RECS))*sizeof(Record), sizeof(Record));
							if (a->id == 0) {
								break;
							}
							if (Sorted_InsertEntry(temp_fd,*a) == -1) {
								return -1;
							}
							k++;
							mergedRecs++;
						}
				}
				else { 				//if (l < ((int)pow(2, (double)(pass-2))*(MAX_RECS))) dhladh uphrxan 2 arxeia kai teleiwse to prwto
					while (l < ((int)pow(2, (double)(pass-2))*(MAX_RECS))) {
						if (l % (MAX_RECS) == 0) {	//gia na mhn diabazw sunexeia to idio block
							if(BF_ReadBlock(temp_fd2, l/(MAX_RECS) , &temp_block2) < 0){
						    	BF_PrintError("Error reading block");
								return -1;
						    }
						}
						memcpy(b, temp_block2 + (l % (MAX_RECS))*sizeof(Record), sizeof(Record));
						if (b->id == 0) {
							break;
						}
						if (Sorted_InsertEntry(temp_fd,*b) == -1) {
							return -1;
						}
						l++;
						mergedRecs++;
					}
				}
				//printf("TEMP%d_%d KAI MERGEDRECS = %d KAI K = %d L = %d\n", i, pass, mergedRecs, k, l);
				prevFiles -= 2;
				if (l) {
					if(BF_CloseFile(temp_fd2) < 0){
						BF_PrintError("Error closing file1");
						return -1;
					}
					if (remove(tempName2) < 0) {
						perror("Error deleting temp files");
						return -1;
					}
					free(tempName2);
				}
				if(BF_CloseFile(temp_fd1) < 0){
					BF_PrintError("Error closing file2");
					return -1;
				}
				if (remove(tempName1) < 0) {
					perror("Error deleting temp files");
					return -1;
				}

				free(tempName1);
			}
			if (totalFiles != 1) {
				if(BF_CloseFile(temp_fd) < 0){
					BF_PrintError("Error closing file");
					return -1;
				}
			}
		}

		if (pass == 1) {
			if(BF_CloseFile(heap_fd) < 0){
				BF_PrintError("Error closing file");
				return -1;
			}
		}

		prevFiles = totalFiles;
		if (totalFiles % 2 == 0) {
			totalFiles = totalFiles/2;
		}
		else{
			totalFiles = totalFiles/2 + 1;
		}
		pass++;
	}while (prevFiles != 1);

	if(Sorted_CreateFile(newName) == -1 ){
		printf("Error creating sorted file!\n");
		return EXIT_FAILURE;
	}
	if( (final_fd = Sorted_OpenFile(newName)) < 0  ){
		printf("Error opening sorted file!\n");
		return EXIT_FAILURE;
	}
	for (int j = 0; j < heapBlocks; j++) {
		if(BF_AllocateBlock(final_fd) < 0){
			BF_PrintError("Error allocating sorted block");
			return -1;
		}
	}

	/*enhmerwsh tou info block*/
	if(BF_ReadBlock(final_fd, 0 , &final_block) < 0){
		BF_PrintError("Error reading block");
		return -1;
	}
	memcpy(&info, final_block, sizeof(S_Info));
	info.fieldNo = fieldNo;
	info.totalRecs = mergedRecs;
	info.mode = mode;
	memcpy(final_block, &info, sizeof(S_Info));

	if(BF_WriteBlock(final_fd , 0) < 0){
		BF_PrintError("Error writing block");
		return -1;
	}

	/*antigrafh apo to teleutaio proswrino sto teliko arxeio*/
	for (int i = 0; i < mergedRecs; i++) {
		if (i % (MAX_RECS) == 0) {
			//printf("ANTIGRAFW APO TO %lu TEMP BLOCK\n", i/(MAX_RECS) );
			if(BF_ReadBlock(temp_fd, i/(MAX_RECS) , &temp_block) < 0){
				BF_PrintError("Error reading block");
				return -1;
			}
			if(BF_ReadBlock(final_fd, (i/(MAX_RECS)) + 1 , &final_block) < 0){
				BF_PrintError("Error reading block");
				return -1;
			}
		}
		memcpy(final_block + (i % (MAX_RECS))*sizeof(Record), temp_block + (i % (MAX_RECS))*sizeof(Record), sizeof(Record));

		if ((i + 1) % (MAX_RECS) == 0 || i == mergedRecs - 1) {
			if(BF_WriteBlock(final_fd , (i/(MAX_RECS)) + 1) < 0){
				BF_PrintError("Error writing block");
				return -1;
			}
		}
	}
	int passDigits = floor(log10(abs(pass-1))) + 1;
	char *tempName;
	if((tempName = (char*) malloc((strlen("temp") + 1 + strlen("_")+ passDigits + 1)*sizeof(char))) == NULL){
		perror("Temp name malloc error");
		return -1;
	}
	sprintf(tempName, "temp%d_%d", 1, pass-1);

	if(BF_CloseFile(temp_fd) < 0){
		BF_PrintError("Error closing file");
		return -1;
	}
	if(BF_CloseFile(final_fd) < 0){
		BF_PrintError("Error closing file");
		return -1;
	}

	if (remove(tempName) < 0) {
		perror("Error removing file");
		return -1;
	}

	free(newName);
	free(tempName);
	return 0;
}

void printRec(Record *rec){
	printf("%d, %s, %s, %s\n",rec->id,rec->name,rec->surname,rec->city);
}


int Sorted_checkSortedFile(char *fileName, int fieldNo){
	int temp_fd;
	void *temp_block;
	S_Info info;
	Record *prev, *curr;

	prev = (Record*) malloc(sizeof(Record));
	curr = (Record*) malloc(sizeof(Record));

	if( (temp_fd = Sorted_OpenFile(fileName)) < 0  ){
        printf("Error opening file!\n");
		return -1;
	}

	if(BF_ReadBlock(temp_fd, 0, &temp_block) < 0){
		BF_PrintError("Error reading block");
		return -1;
	}
	memcpy(&info, temp_block, sizeof(S_Info));
	if (info.type != 'S') {
		printf("Not a 'sorted' type of file!\n");
		return -1;
	}
	else if (info.fieldNo == -1) {
		printf("This file has not been sorted yet!\n");
		return -1;
	}

	if(BF_ReadBlock(temp_fd, 1, &temp_block) < 0){
		BF_PrintError("Error reading block");
		return -1;
	}
	memcpy(prev, temp_block, sizeof(Record));
	for (int i = 1; i < info.totalRecs; i++) {
		if (i % (MAX_RECS) == 0) {
			if(BF_ReadBlock(temp_fd, i/(MAX_RECS) + 1, &temp_block) < 0){
				BF_PrintError("Error reading block");
				return -1;
			}
		}
		memcpy(curr, temp_block + (i%(MAX_RECS))*sizeof(Record), sizeof(Record));
		if (recordCompare(prev, curr, fieldNo, info.mode) == 1) {
			if(Sorted_CloseFile(temp_fd) < 0){
				BF_PrintError("Error closing file");
				return -1;
			}
			return -1;
		}
		prev = curr;
	}
	printf("File is sorted!\n");
	if(Sorted_CloseFile(temp_fd) < 0){
		BF_PrintError("Error closing file");
		return -1;
	}
	return 0;
}


int Sorted_GetAllEntries(int fileDesc, int *fieldNo, void *value){
	void *sorted_block;
	int printBlocks = BF_GetBlockCounter(fileDesc) - 1;
	printf("\nSearching...\n");
	if (value == NULL) {
		for (int m = 1; m <= printBlocks; m++) {
			printf("\nPrinting %d block from sorted file\n",m );
			if(BF_ReadBlock(fileDesc, m, &sorted_block) < 0){
				BF_PrintError("Error reading block");
				return -1;
			}
			for (int j = 0; j < (MAX_RECS); j++) {
				Record rec;
				memcpy(&rec, sorted_block + j*sizeof(Record), sizeof(Record));
				printRec(&rec);
			}
		}
		printf("\nBlocks read : %d\n", printBlocks);
	}
	else{
		int first, last, middle, blocksRead = 0, recsFound = 0;
		void *m_block, *block;
		S_Info info;
		Record *a, *b;

		a = (Record*) malloc(sizeof(Record));
		b = (Record*) malloc(sizeof(Record));

		if(BF_ReadBlock(fileDesc, 0, &m_block) < 0){
			BF_PrintError("Error reading block");
			return -1;
		}
		memcpy(&info, m_block, sizeof(info));

		first = 0;
		last = info.totalRecs - 1;
		middle = (first+last)/2;

		while(first <= last){
			if(BF_ReadBlock(fileDesc, middle/(MAX_RECS) + 1, &m_block) < 0){
				BF_PrintError("Error reading block");
				return -1;
			}
			memcpy(a, m_block + (middle % (MAX_RECS))*sizeof(Record), sizeof(Record));
			blocksRead++;

			if (*fieldNo == 0) {
				if (a->id < *(int*)value) {
					first = middle + 1;
				}
				else if (a->id == *(int*)value) {
					recsFound++;
					printf("\nRecords found :\n");
					printRec(a);
					break;
				}
				else{
					last = middle - 1;
				}
			}
			else if (*fieldNo == 1){
				if (strcmp(a->name, (char*)value) < 0) {
					first = middle + 1;
				}
				else if (strcmp(a->name, (char*)value) == 0) {
					printf("\nRecords found :\n");
					block = m_block;
					for (int i = middle - 1; i >= first; i--) {
						if (i % (MAX_RECS) == (MAX_RECS - 1)) {
							if(BF_ReadBlock(fileDesc, i/(MAX_RECS) + 1, &block) < 0){
								BF_PrintError("Error reading block");
								return -1;
							}
							blocksRead++;
						}
						memcpy(b, block + (i % (MAX_RECS))*sizeof(Record), sizeof(Record));
						if (strcmp(b->name, a->name) == 0) {
							recsFound++;
							printRec(b);
						}
						else{
							break;
						}
					}
					recsFound++;
					printRec(a);
					block = m_block;
					for (int i = middle + 1; i <= last; i++) {
						if (i % (MAX_RECS) == 0) {
							if(BF_ReadBlock(fileDesc, i/(MAX_RECS) + 1, &block) < 0){
								BF_PrintError("Error reading block");
								return -1;
							}
							blocksRead++;
						}
						memcpy(b, block + (i % (MAX_RECS))*sizeof(Record), sizeof(Record));
						if (strcmp(b->name, a->name) == 0) {
							recsFound++;
							printRec(b);
						}
						else{
							break;
						}
					}
					break;
				}
				else{
					last = middle - 1;
				}
			}
			else if (*fieldNo == 2) {
				if (strcmp(a->surname, (char*)value) < 0) {
					first = middle + 1;
				}
				else if (strcmp(a->surname, (char*)value) == 0) {
					printf("\nRecords found :\n");
					block = m_block;
					for (int i = middle - 1; i >= first; i--) {
						if (i % (MAX_RECS) == (MAX_RECS - 1)) {
							if(BF_ReadBlock(fileDesc, i/(MAX_RECS) + 1, &block) < 0){
								BF_PrintError("Error reading block");
								return -1;
							}
							blocksRead++;
						}
						memcpy(b, block + (i % (MAX_RECS))*sizeof(Record), sizeof(Record));
						if (strcmp(b->surname, a->surname) == 0) {
							recsFound++;
							printRec(b);
						}
						else{
							break;
						}
					}
					recsFound++;
					printRec(a);
					block = m_block;
					for (int i = middle + 1; i <= last; i++) {
						if (i % (MAX_RECS) == 0) {
							if(BF_ReadBlock(fileDesc, i/(MAX_RECS) + 1, &block) < 0){
								BF_PrintError("Error reading block");
								return -1;
							}
							blocksRead++;
						}
						memcpy(b, block + (i % (MAX_RECS))*sizeof(Record), sizeof(Record));
						if (strcmp(b->surname, a->surname) == 0) {
							recsFound++;
							printRec(b);
						}
						else{
							break;
						}
					}
					break;
				}
				else{
					last = middle - 1;
				}
			}
			else if (*fieldNo == 3) {
				if (strcmp(a->city, (char*)value) < 0) {
					first = middle + 1;
				}
				else if (strcmp(a->city, (char*)value) == 0) {
					printf("\nRecords found :\n");
					block = m_block;
					for (int i = middle - 1; i >= first; i--) {
						if (i % (MAX_RECS) == (MAX_RECS - 1)) {
							if(BF_ReadBlock(fileDesc, i/(MAX_RECS) + 1, &block) < 0){
								BF_PrintError("Error reading block");
								return -1;
							}
							blocksRead++;
						}
						memcpy(b, block + (i % (MAX_RECS))*sizeof(Record), sizeof(Record));
						if (strcmp(b->city, a->city) == 0) {
							recsFound++;
							printRec(b);
						}
						else{
							break;
						}
					}
					recsFound++;
					printRec(a);
					block = m_block;
					for (int i = middle + 1; i <= last; i++) {
						if (i % (MAX_RECS) == 0) {
							if(BF_ReadBlock(fileDesc, i/(MAX_RECS) + 1, &block) < 0){
								BF_PrintError("Error reading block");
								return -1;
							}
							blocksRead++;
						}
						memcpy(b, block + (i % (MAX_RECS))*sizeof(Record), sizeof(Record));
						if (strcmp(b->city, a->city) == 0) {
							recsFound++;
							printRec(b);
						}
						else{
							break;
						}
					}
					break;
				}
				else{
					last = middle - 1;
				}
			}
			middle = (first+last)/2;
		}
		if (first > last) {
			printf("\nRequested record wasn't found!\n");
		}
		printf("\nAmount of records found : %d\n",recsFound );
		printf("Blocks read for search : %d\n",blocksRead );
	}
	return 0;
}
