#include "output.h"

void output(char *outputFile,int answerSize,int* answer){

	std::ofstream out(outputFile);
	out<<"Colors used: "<<answerSize<<"\n";
	for(int i=0;i<vertexNumber;i++){
		out<<answer[i];
	}
	delete answer;
	out<<'\n';
}
