all:
	clang -Iutil jjvm/*.c src/*.c util/*.c -Wall -Wextra -O0 -g -std=gnu99

tests:
	clang -Iutil jjvm/*.c src/*.c util/*.c -Wall -Wextra -O0 -g -std=gnu99
	./run_tests.sh