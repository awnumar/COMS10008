all:
	@ make -s test

build:
	@ make -s clean
	@ gcc -std=c99 -Wall -O3 -pedantic -c ./list.c -o ./list.o

test:
	@ make -s clean
	@ gcc -std=c99 -Wall -O3 -pedantic ./list.c -o ./list
	@ ./list
	@ make -s clean

clean:
	@ rm -rf ./a.out ./list.h.gch ./list.o ./list
