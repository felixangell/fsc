const void 
blah() {
	if (52) {

	}
}

const int* foo() {
	if (1) {

	} else if (2) {

	} else {

	}
	return 0;	
}

const int main() {
	int x = 15;
	int y = x * 2;
	int* xptr = &x;
	// printf("Value at %p is %d!\n", xptr, x);
	*xptr = y + y;
	// printf("Value at %p is %d!\n", xptr, x);
	return 0;
}