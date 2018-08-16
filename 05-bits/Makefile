all:
	@ make -s run

build:
	gcc -std=c99 -Wall -O3 -pedantic ./bits.c -o ./bits

run:
	@ make -s clean
	@ make -s build
	@ ./bits

clean:
	rm -rf ./bits
