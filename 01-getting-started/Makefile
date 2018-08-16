all:
	@ make -s run

build:
	gcc -std=c99 -Wall -O3 ./hello.c -o ./hello

run:
	@ make -s clean
	@ make -s build
	@ ./hello

clean:
	rm -rf ./hello
