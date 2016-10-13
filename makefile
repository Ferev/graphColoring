EXECS=antcol genetic
DEPENDENCIES=input output
MPICXX=mpicxx
INCLUDEDIR=-I/home/ivan/graphColoringProblem/include/
CFLAGS= ${INCLUDEDIR} -O2 --std=c++11


.PHONY: all input output antcol genetic
all:${EXECS}

input:
	${MPICXX} ${CFLAGS} src/input/input.cpp -c -o src/input/input.o

output:
	${MPICXX} ${CFLAGS} src/output/output.cpp -c -o src/output/output.o

antcol: ${DEPENDENCIES}
	cd src/antcol; make
	${MPICXX} ${CFLAGS} -o bin/antcol src/input/input.o src/output/output.o src/antcol/antcol.o

genetic: ${DEPENDENCIES}
	cd src/genetic; make
	${MPICXX} ${CFLAGS} -o bin/genetic src/input/input.o src/output/output.o src/genetic/genetic.o

cleanTemporary:
	cd src/antcol; make clean;
	cd ../genetic; make clean;
	rm -rf src/input/input.o
	rm -rf src/output/ouput.o

clean: cleanTemporary
	rm bin/${EXECS}
