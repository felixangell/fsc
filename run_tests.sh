#!/bin/sh
echo "running tests..."

for test_file in tests/*.yava; do
	./a.out "$test_file" >/dev/null
done
