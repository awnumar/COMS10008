build:
	@ make -s clean
	@ gcc -std=c99 -Wall -O3 -pedantic -c ./ctime.c -o ./ctime.o

test:
	@ make -s clean
	@ gcc -std=c99 -Wall -O3 -pedantic ./ctime.c -o ./ctime
	@ ./ctime
	@ make -s clean

clean:
	@ rm -rf ./a.out ./ctime.h.gch ./ctime.o ./ctime
