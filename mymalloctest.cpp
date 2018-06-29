
#include <stdio.h>
#include <stdlib.h>
#include "header.h"
int main(){

	char *c = (char *)dtr_malloc(sizeof(char) * 10);

	printf("C: %d\n", c);

	dtr_free(c);

	displayBlob();

	char *b = (char *)dtr_malloc(sizeof(char) * 10);

	printf("B: %d\n", b);

	dtr_free(b);

	displayBlob();

	char *a = (char *)dtr_malloc(sizeof(char) * 10);

	printf("A: %d\n", a);

	dtr_free(a);

	displayBlob();

	char *d = (char *)dtr_malloc(sizeof(char) * 10);

	printf("D: %d\n", d);

	displayBlob();

	system("pause");
	return 0;
}