#include "node.h"

double** currentMemory;
std::set<int>* suitableNeighbors;
std::set<int> candidatesForColor,cannotBeColored;
std::vector<std::vector<int>> coloring;

int desirability(int vertex,int choosingDesirability){
	if(choosingDesirability==0){
		int desirabilityOfVertex=0;
		for(int i=0;i<neighbors[vertex].size();i++){
			if(cannotBeColored.find(neighbors[vertex][i])!=cannotBeColored.end()){
				desirabilityOfVertex++;
			}
		}
		return desirabilityOfVertex;
	}
	if(choosingDesirability==1){
		int counter=0;
		for(int i=0;i<neighbors[vertex].size();i++){
			if(candidatesForColor.find(neighbors[vertex][i])!=candidatesForColor.end()){
				counter++;
			}
		}
		return candidatesForColor.size()-counter;
	}
	int desirabilityOfVertex=0;
	for(int i=0;i<neighbors[vertex].size();i++){
		if(cannotBeColored.find(neighbors[vertex][i])!=cannotBeColored.end()
		   ||candidatesForColor.find(neighbors[vertex][i])!=candidatesForColor.end()){
			desirabilityOfVertex++;
		}
	}
	return desirabilityOfVertex;
}

double trailFactor(int vertex,int color){
	if(coloring[color].size()==0){
		return 1.0;
	}else{
		double trailFactorForVertex=0.0;
		for(int i=0;i<coloring[color].size();i++){
			trailFactorForVertex+=currentMemory[coloring[color][i]][vertex];
		}
		return trailFactorForVertex/coloring[color].size();
	}
}

double probability(int vertex,int color,int choosingDesirability){
	return desirability(vertex,choosingDesirability)*trailFactor(vertex,color);
}

void recursiveLargestFirst(){
	suitableNeighbors=new std::set<int>[vertexNumber];
	double* probabilities=new double[vertexNumber];
	currentMemory=new double*[vertexNumber];
	for(int i=0;i<vertexNumber;i++){
		currentMemory[i]=new double[vertexNumber];
		MPI_Bcast(currentMemory[i],vertexNumber,MPI_DOUBLE,masterNode,MPI_COMM_WORLD);
	}

	std::mt19937_64 gen(std::random_device{}());
	std::uniform_int_distribution<unsigned> parametersGenerator(1,6);
	int choosingFirstVertex=parametersGenerator(gen)%2;
	int choosingDesirability=parametersGenerator(gen)%3;
	
	int numberOfColors=0;
	candidatesForColor.clear();
	for(int i=0;i<vertexNumber;i++){
		candidatesForColor.insert(i);
	}

	int numberOfColored=0;
	coloring.clear();
	std::vector<int> newColor;
	coloring.push_back(newColor);
	while(numberOfColored<vertexNumber){
		numberOfColored++;
		numberOfColors++;
		cannotBeColored.clear();
		int vertex;
		/*printf("Vertices in W: ");
		for(auto vert: candidatesForColor){
			printf("%d ",vert+1);
		}
		printf("\n");*/
		if(choosingFirstVertex==0){
			int maxVertexDegree=-1;
			for(auto candidate: candidatesForColor){
				int counter=0;
				for(int i=0;i<neighbors[candidate].size();i++){
					if(candidatesForColor.find(neighbors[candidate][i])!=candidatesForColor.end()){
						counter++;
					}
				}
				if(counter>maxVertexDegree){
					maxVertexDegree=counter;
					vertex=candidate;
				}
			}
		}
		else{
			std::uniform_int_distribution<unsigned> randomVertex(0,candidatesForColor.size()-1);
			int indexOfVertex=randomVertex(gen);
			auto placeOfVertex=candidatesForColor.begin();
			for(int i=0;i<indexOfVertex;i++){
				placeOfVertex++;
			}
			vertex=(*placeOfVertex);
		}
		//printf("Number %d, Vertex %d, Color %d\n",numberOfColored,vertex+1,numberOfColors);
		coloring.push_back(newColor);
		coloring[numberOfColors].push_back(vertex);
		while(true){
			int counter=1;
			for(int i=0;i<neighbors[vertex].size();i++){
				if(candidatesForColor.find(neighbors[vertex][i])!=candidatesForColor.end()){
					counter++;
				}
			}
			if(candidatesForColor.size()<=counter){
				break;
			}
			numberOfColored++;
			candidatesForColor.erase(vertex);
			for(int i=0;i<neighbors[vertex].size();i++){
				if(candidatesForColor.find(neighbors[vertex][i])!=candidatesForColor.end()){
					cannotBeColored.insert(neighbors[vertex][i]);
					candidatesForColor.erase(neighbors[vertex][i]);
				}
			}
			double sumOfProbabilities=0.0;
			for(auto candidate: candidatesForColor){
				probabilities[candidate]=probability(candidate,numberOfColors,choosingDesirability);
				sumOfProbabilities+=probabilities[candidate];
			}
			std::uniform_real_distribution<double> chooseNext(0.0,sumOfProbabilities);
			double nextVertex=chooseNext(gen),currentSum=0.0;
			vertex=(*candidatesForColor.begin());
			//printf("Sum %f, chosen %f\n",sumOfProbabilities,nextVertex);
			for(auto candidate: candidatesForColor){
				currentSum+=probabilities[candidate];
				if(currentSum>nextVertex){
					vertex=candidate;
					break;
				}
			}
			/*printf("Vertices in W: ");
			for(auto vert: candidatesForColor){
				printf("%d ",vert+1);
			}
			printf("\n");
			printf("Number %d, Vertex %d, Color %d\n",numberOfColored,vertex+1,numberOfColors);*/
			coloring[numberOfColors].push_back(vertex);
		}
		//printf("New color\n");
		std::set<int> mem=candidatesForColor;
		candidatesForColor.clear();
		for(auto impossibleVertex: cannotBeColored){
			//printf("Vertex %d from B\n",impossibleVertex+1);
			candidatesForColor.insert(impossibleVertex);
		}
		for(int i=0;i<neighbors[vertex].size();i++){
			if(mem.find(neighbors[vertex][i])!=mem.end()){
				//printf("Vertex %d from Nw\n",neighbors[vertex][i]+1);
				candidatesForColor.insert(neighbors[vertex][i]);
			}
		}

	}
	
	delete [] probabilities;
	delete [] currentMemory;
	delete [] suitableNeighbors;
}

void nodeMain(){
	int triggerSignal;
	int cnt=0;
	while(true){
		MPI_Recv(&triggerSignal,1,MPI_INT,masterNode,constructSolutionTag,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		if(triggerSignal==-1) break;
		recursiveLargestFirst();
		int numberOfColors=coloring.size()-1;
		MPI_Send(&numberOfColors,1,MPI_INT,masterNode,sendSolutionTag,MPI_COMM_WORLD);
		for(int i=1;i<=numberOfColors;i++){
			for(int j=0;j<coloring[i].size();j++){
				//printf("%d ",coloring[i][j]+1);
			}
			MPI_Send(&coloring[i][0],coloring[i].size(),MPI_INT,masterNode,sendSolutionTag,MPI_COMM_WORLD);
		}
		cnt++;
	}
}
