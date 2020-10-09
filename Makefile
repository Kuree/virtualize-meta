all: test-all

parser.so: parser.c parser.h
	gcc -Werror -Wall parser.c -shared -o parser.so -fPIC

test: test.c parser.so
	gcc -Werror -Wall test.c parser.so -o test

test-tb: cgra_info_pkg.sv tb.sv
	xrun cgra_info_pkg.sv tb.sv -sv_lib parser.so

test-all: test-tb test
	LD_LIBRARY_PATH=. ./test
