all:
	@ make -s run

build:
	gcc -std=c99 -Wall -O3 checksum.c -lssl -lcrypto -o checksum

run:
	@ make -s clean
	@ make -s build
	@ ./checksum

clean:
	rm -rf ./checksum
