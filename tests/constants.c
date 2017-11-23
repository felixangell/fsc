struct s { int i; };

int f(void) {
	struct s *p = 0, *q;
	int j = 0;

again:
	q = p, p = &((struct s) { j++ });
	if (j < 2) goto again;
	return p == q && q->i == 1;
}

void t(int n, int * restrict p, int * restrict q) {
	while (n-- > 0) {
		*p++ = *q++;
	}
}

inline double fahr(double t) {
	return (9.0 * t) / 5.0 + 32.0;
}

inline double cels(double t) {
	return (5.0 * (t - 32.0)) / 9.0;
}

double convert(int is_fahr, double temp) {
	return is_fahr ? cels(temp) : fahr(temp);
}

int main(int argc, char** argv) {
	int* x = &argc;
	*x = (a / b) * b + a % b;

	static int i = 2 || 1 / 0;

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