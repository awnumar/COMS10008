all:
	@ make -s run

build:
	gcc -std=c99 -Wall -O3 -pedantic ./triangle.c -o ./triangle

run:
	@ make -s clean
	@ make -s build
	@ ./triangle

clean:
	rm -rf ./triangle
