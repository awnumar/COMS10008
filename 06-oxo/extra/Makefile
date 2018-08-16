all:
	@ make -s run

build:
	gcc -std=c99 -Wall -O3 -pedantic ./server.c -lpthread -o ./server

run:
	@ make -s clean
	@ make -s build
	@ ./server

clean:
	rm -rf ./server
