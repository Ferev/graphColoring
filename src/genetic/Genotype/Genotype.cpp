#include "Genotype.h"

Genotype::Genotype(){
	//colors.resize(vertexNumber);
}

void Genotype::setFitness(int fitness){
	this->fitness=fitness;
}

int Genotype::getFitness(){
	return fitness;
}

void Genotype::set(int index,int color){
	colors[index]=color;
}

int Genotype::get(int index){
	return colors[index];
}

bool operator<(Genotype a,Genotype b){
	return a.getFitness()<b.getFitness();
}
