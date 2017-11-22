struct s { int i; };

int f(void) {
	struct s *p = 0, *q;
	int j = 0;

again:
	q = p, p = &((struct s) { j++ });
	if (j < 2) goto again;
	return p == q && q->i == 1;
}

int main(int argc, char** argv) {
	int* x = &argc;
	*x = (a / b) * b + a % b;

	{ 
		int n = 4, m = 3;
		int a[n][m];
		int (*p)[m] = a;
		p += 1;
		(*p)[2] = 99;
		n = p - a;
	}

	return 0;
}