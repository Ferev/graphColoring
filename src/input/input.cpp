#include "input.h"

const int MaxVertexNumber=4000;
const char commentSymbol='c';
const char graphNodesSymbol='p';
const char edgeSymbol='e';

int vertexNumber;
int edgesNumber;
std::vector<int> neighbors[MaxVertexNumber];

void readInput(char *inputFile){
	char beginningSymbol;
	std::ifstream input(inputFile);
	input.get(beginningSymbol);
	while(beginningSymbol==commentSymbol){
		input.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
		input.get(beginningSymbol);
	}
	if(beginningSymbol==graphNodesSymbol){
		input.ignore(std::numeric_limits<std::streamsize>::max(),' ');
		input.ignore(std::numeric_limits<std::streamsize>::max(),' ');
		input>>vertexNumber;
		input>>edgesNumber;
	}
	for(int i=0;i<edgesNumber;i++){
		input>>beginningSymbol;
		if(beginningSymbol==edgeSymbol){
			int firstVertex,secondVertex;
			input>>firstVertex;
			input>>secondVertex;
			firstVertex--;
			secondVertex--;
			neighbors[firstVertex].push_back(secondVertex);
			neighbors[secondVertex].push_back(firstVertex);
		}
	}
}
