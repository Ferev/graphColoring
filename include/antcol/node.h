#ifndef __NODE_INCLUDED__
#define __NODE_INCLUDED__

#include<vector>
#include<set>
#include<mpi.h>
#include<random>
#include "input.h"
#include "constants.h"

extern double** currentMemory;
extern std::set<int>* suitableNeighbors;
extern std::set<int> candidatesForColor,cannotBeColored;
extern std::vector<std::vector<int>> coloring;

int desirability(int vertex,int choosingDesirability);

double trailFactor(int vertex,int color);

double probability(int vertex,int color,int choosingDesirability);

void recursiveLargestFirst();

void nodeMain();
#endif 
