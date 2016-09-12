#ifndef __MASTER_INCLUDED__
#define __MASTER_INCLUDED__

#include<vector>
#include<mpi.h>
#include "input.h"
#include "output.h"
#include "constants.h"

extern const int cyclesMultiplier;
extern double** memory;
extern double** deltaMemory;
extern const double evaporationRate;
extern int** solution,finalSolution;
extern bool** neighborsScheme;

void initializeMemory();

void initializeDeltaMemory();

void sendMToNodes();

void makeNeighborScheme();

void destructor();

void masterMain(char* outputFile);
#endif 
