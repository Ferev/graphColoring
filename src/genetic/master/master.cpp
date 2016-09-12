#include<stdint.h>
#include<random>
#include "master.h"


const int populationMultiplier=10;
const int generationMultiplier=10;
const int roundsOfProcessing=10;
const int minimalRandomShuffleBlock=2;

Genotype lastSuccessfulColoring;
Genotype* population;
Genotype* GMPopulation;

bool possibleColoring(int numberOfColors){

	int generationsNumber=vertexNumber*generationMultiplier;
	int populationSize=vertexNumber*populationMultiplier;
	int blockSize=populationSize/roundsOfProcessing;
	
	population=new Genotype[populationSize];
	GMPopulation=new Genotype[roundsOfProcessing];
	
	std::mt19937_64 gen(std::random_device{}());
	std::uniform_int_distribution<int> randomShuffleBlock(minimalRandomShuffleBlock,blockSize);
	
	//printf("StartGM\n");
	communication::startGM(roundsOfProcessing,numberOfColors);
	//printf("Creating population\n");
	communication::sendCreate(population,blockSize,roundsOfProcessing,numberOfColors);
	for(int i=0;i<generationsNumber;i++){
		
		if(i%10==0) printf("Generation: %d\n",i);
		//printf("Reproducing\n");
		communication::sendReproduce(population,blockSize,roundsOfProcessing);
		//printf("Mutating\n");
		communication::sendMutate(population,blockSize,roundsOfProcessing);
		//printf("CheckignGM\n");
		if(communication::checkGM()){
			//printf("ReceivingGM\n");
			communication::receiveGM(GMPopulation);
			//printf("StartingGM\n");
			communication::startGM(roundsOfProcessing,numberOfColors);
			//printf("SendingNodesGM\n");
			communication::sendNodesGM(population,GMPopulation,blockSize,roundsOfProcessing);
		}
		//communication::sendRandom(population,randomShuffleBlock(gen),roundsOfProcessing);
		//printf("Checking for solution\n");
		if(communication::checkSolution(numberOfColors,lastSuccessfulColoring)){
			for(int i=0;i<vertexNumber;i++){
				printf("%d ",lastSuccessfulColoring.get(i));
			}
			printf("\n");
			//output(lastSuccessfulColoring);
			delete[] population;
			return true;
		}
	}
	delete[] population;
	delete[] GMPopulation;
	return false;
}

void masterMain(char *outputFile){
	int lowestColorNumber=1,biggestColorNumber=0,middleColorNumber;
	for(int i=0;i<vertexNumber;i++){
		if(biggestColorNumber<neighbors[i].size()+1){
			biggestColorNumber=neighbors[i].size()+1;
		}
	}
	while(lowestColorNumber<biggestColorNumber){
		middleColorNumber=(lowestColorNumber+biggestColorNumber)/2;
		printf("Checking %d\n",middleColorNumber);
		if(possibleColoring(middleColorNumber)){
			biggestColorNumber=middleColorNumber;
		}else{
			lowestColorNumber=middleColorNumber+1;
		}
	}

	int* answerColoring=new int[vertexNumber];
	for(int i=0;i<vertexNumber;i++){
		answerColoring[i]=lastSuccessfulColoring.get(i);
	}
	output(outputFile, biggestColorNumber, answerColoring);
	communication::suspendAllThreads();
}
