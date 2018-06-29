// MyMalloc.cpp : Defines the exported functions for the DLL application.
// MyMalloc.h : Contains the declaration of the functions of DLL application.


#include <stdio.h>
#include <stdlib.h>
#include "header.h"

typedef struct FreedData{
	int size;
	void *address;
	FreedData *next;
};

typedef struct BLOB{
	int size;
	void *startAddress;
	void *endAddress;
	void *memoryAddress;
	FreedData *freedDataHead;
}BLOB;

BLOB blob[NUMBER_OF_PAGES];
int count = -1;

void deleteNode(FreedData *node){
	FreedData *tempNode = blob[count].freedDataHead, *prevNode = NULL;

	if (tempNode->address == node->address){
		blob[count].freedDataHead = blob[count].freedDataHead->next;
		return;
	}

	while (tempNode != NULL){
		if (tempNode->address == node->address){
			prevNode->next = tempNode->next;
			free(tempNode);
			break;
		}
		prevNode = tempNode;
		tempNode = tempNode->next;
	}
}

void adjustFreedDataNode(FreedData *node, int size){
	if (node->size == size)
		deleteNode(node);
	else{
		node->size -= size;
		node->address = (char *)node->address + size;
	}
}

void* checkFreedDataSize(int index, int size){
	FreedData *tempNode = blob[index].freedDataHead;
	while (tempNode != NULL){
		if (tempNode->size >= size){
			short *temp = (short *)tempNode->address;
			temp -= 1;
			temp[0] = (short)size;
			void *returnAddress = (char *)tempNode->address + OFFSET;
			adjustFreedDataNode(tempNode, size);
			return returnAddress;
		}
		tempNode = tempNode->next;
	}
	return NULL;
}

void *allocateMemory(int index, int size){
	short *temp = (short *)blob[index].memoryAddress;
	temp[0] = (short)size;
	void *returnAddress = (char *)blob[index].memoryAddress + OFFSET;
	blob[index].memoryAddress = (char *)blob[index].memoryAddress + size;

	return returnAddress;
}

void initialize(int count){
	blob[count].size = 0;
	blob[count].memoryAddress = malloc(1024);
	blob[count].startAddress = blob[count].memoryAddress;
	blob[count].endAddress = (char *)blob[count].startAddress + PAGE_SIZE;
	blob[count].freedDataHead = NULL;
}

void *returnAddress(int count, int size){

	void *returnAddress = checkFreedDataSize(count, size);

	if (returnAddress)
		return returnAddress;

	return allocateMemory(count, size);
}

FreedData *createNewNode(int size, void *address){
	FreedData *newNode = (FreedData *)malloc(sizeof(FreedData));
	newNode->size = size;
	newNode->address = address;
	newNode->next;

	return newNode;
}

void addFreeList(int count, int size, void *address){
	FreedData *newNode = createNewNode(size, address);

	FreedData *tempNode = blob[count].freedDataHead;

	if (tempNode == NULL || tempNode->address >= newNode->address){
		newNode->next = blob[count].freedDataHead;
		blob[count].freedDataHead = newNode;
		return;
	}

	while (tempNode->next != NULL && tempNode->address < newNode->address)
		tempNode = tempNode->next;

	newNode->next = tempNode->next;
	tempNode->next = newNode;

	if ((char *)tempNode->address + tempNode->size == newNode->address){
		tempNode->size += newNode->size;
		tempNode->next = newNode->next;
		newNode = tempNode;
	}
	if (newNode->next != NULL && (char *)newNode->address + newNode->size == newNode->next->address){
		newNode->size += newNode->next->size;
		newNode->next = newNode->next->next;
		free(newNode->next);
	}
}

void *dtr_malloc(int size){

	int freeBlob = -1;

	if (count == -1)
		initialize(++count);

	for (int i = 0; i < count; ++i){
		if (blob[i].size + size + OFFSET <= PAGE_SIZE){
			freeBlob = i;
			break;
		}
	}

	if (freeBlob != -1)
		return returnAddress(freeBlob, size);

	if (blob[count].size + size + OFFSET > PAGE_SIZE){
		count += 1;
		if (count >= 12 || size > PAGE_SIZE){
			printf("\nOut of memory.\n");
			return NULL;
		}
		initialize(count);
	}

	blob[count].size += size + OFFSET;

	return returnAddress(count, size);
}

void dtr_free(void *address){

	int blobIndex = -1;
	short *temp = (short *)address;
	temp -= 1;
	int size = temp[0];
	address = (void *)temp;

	for (int i = 0; i <= count; ++i){
		if (blob[i].size == 0 && blob[i].memoryAddress != NULL){
			free(blob[i].startAddress);
			blob[i] = { 0 };
		}
	}

	for (int i = 0; i <= count; ++i){
		if (blob[i].startAddress <= address && address <= blob[i].endAddress){
			blobIndex = i;
			blob[blobIndex].size -= (size + OFFSET);
			break;
		}
	}

	addFreeList(blobIndex, size, address);
}

void displayBlob(){
	for (int i = 0; i <= count; ++i){
		printf("\n\nBLOB: 1\n\n");
		FreedData *tempNode = blob[i].freedDataHead;
		if (tempNode == NULL){
			printf("Data is not freed yet.\n");
			continue;
		}
		while (tempNode != NULL){
			printf("Size: %5d\nAddress: %5d\n", tempNode->size, tempNode->address);
			tempNode = tempNode->next;
		}
	}
}