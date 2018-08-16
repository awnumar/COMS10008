all:
	@ make -s run

build:
	gcc -std=c99 -Wall -O3 ./grade.c -o ./grade

run:
	@ make -s clean
	@ make -s build
	@ ./grade

clean:
	rm -rf ./grade
