all:
	clang -Iutil src/*.c util/*.c -Wall -Wextra -std=gnu99
