#include <stdio.h>


#define OFFSET 2
#define PAGE_SIZE 1024
#define NUMBER_OF_PAGES 12


extern "C"
{
	__declspec(dllexport) void* dtr_malloc(int);

	__declspec(dllexport) void dtr_free(void *);

	__declspec(dllexport) void dtr_debug();

	__declspec(dllexport) void displayBlob();
}