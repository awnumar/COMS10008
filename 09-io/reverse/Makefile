all:
	@ make -s run

build:
	gcc -std=c99 -Wall -O3 -pedantic ./reverse.c -o ./reverse

run:
	@ make -s clean
	@ make -s build
	@ ./reverse
	@ make -s clean

clean:
	rm -rf ./reverse
