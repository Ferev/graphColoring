#include<vector>
#include "input.h"

#ifndef __GENOTYPE_H_INCLUDED__
#define __GENOTYPE_H_INCLUDED__

class Genotype{

	private:
		//std::vector<int> colors;
		int colors[4001];
		int fitness;

	public:
		Genotype();

		void setFitness(int fitness);

		int getFitness();

		void set(int index,int color);

		int get(int index);
};

bool operator<(Genotype a,Genotype b);
#endif
