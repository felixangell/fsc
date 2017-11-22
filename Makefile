src = $(wildcard *.c)
obj = $(patsubst %.c,%.o,$(src))
out := lcc

CCFLAGS = -Wall -Wextra -g3 -std=c99

print-%  : ; @echo $* = $($*)

default: $(out)

%.o: %.c
	$(CC) -c $< -o $@

$(out): $(obj)
	$(CC) $(obj) -o $@

self: $(out)
	./$(out) $(src)

clean:
	-rm -f *.ppc
	-rm -f $(obj)
	-rm -f $(out)