all:
	@ make -s run

build:
	gcc -std=c99 -Wall -O3 -pedantic ./oxo.c -o ./oxo

run:
	@ make -s clean
	@ make -s build
	@ ./oxo

clean:
	rm -rf ./oxo
