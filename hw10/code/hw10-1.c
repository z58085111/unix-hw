#include <stdio.h>

int main(void)
{
	int x = 1;
	char *y = (char *)&x;
	
	if(*y == 0) {
		printf("Big endian.\n");
	} else if(*y == 1) {
		printf("Little endian.\n");
	}
	return 0;
}
