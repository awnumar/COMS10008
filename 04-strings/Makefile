all:
	@ make -s run

build:
	gcc -std=c99 -Wall -O3 -pedantic ./string.c -o ./string

run:
	@ make -s clean
	@ make -s build
	@ ./string

clean:
	rm -rf ./string
