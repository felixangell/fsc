all:
	clang -Iutil jjvm/*.c src/*.c util/*.c -Wall -Wextra -std=gnu99