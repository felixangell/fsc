#include <stdio.h>

int main() {
	int x = 15;
	int y = x * 2;
	int* xptr = &x;
	// printf("Value at %p is %d!\n", xptr, x);
	*xptr = y + y;
	// printf("Value at %p is %d!\n", xptr, x);
	return 0;
}