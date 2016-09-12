#include "master.h"

const int cyclesMultiplier = 10;

double** memory;
double** deltaMemory;

const double evaporationRate = 0.5;

//std::vector<std::vector<int>> solution,finalSolution;
int** solution,finalSolution;
bool** neighborsScheme;

void initializeMemory(){
	memory=new double*[vertexNumber];
	for(int i=0; i<vertexNumber; i++){
		memory[i]=new double[vertexNumber];
		for(int j=0;j<vertexNumber;j++) memory[i][j] = 1;
	}
}

void initializeDeltaMemory(){
	deltaMemory=new double*[vertexNumber];
	for(int i=0; i<vertexNumber; i++){
		deltaMemory[i]=new double[vertexNumber];
		for(int j=0;j<vertexNumber;j++) deltaMemory[i][j] = 0;
	}
}

void destructor(){
	delete memory;
	delete deltaMemory;
	delete neighborsScheme;
}

void sendMToNodes(){
	for(int i=0;i<vertexNumber;i++){
		MPI_Bcast(memory[i],vertexNumber,MPI_DOUBLE, masterNode, MPI_COMM_WORLD);
	}
}

void makeNeighborScheme(){
	neighborsScheme=new bool*[vertexNumber];
	for(int i=0; i<vertexNumber; i++){
		neighborsScheme[i]=new bool[vertexNumber];
		for(int j=0; j<neighbors[i].size(); j++) neighborsScheme[i][j] = 1;
	}
}

void masterMain(char* outputFile){
	int numberOfProcesses;
	MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);

	initializeMemory();
	makeNeighborScheme();

	int mySolution[1001][1001];
	int answer=vertexNumber;
	int numberOfCycles=cyclesMultiplier*vertexNumber;
	int* finalSolution=new int[vertexNumber];

	for(int i=0; i<numberOfCycles; i++){
		initializeDeltaMemory();
		for(int j=1; j<numberOfProcesses; j++){
			int triggerMessage=1;
			MPI_Send(&triggerMessage,1,MPI_INT,j,constructSolutionTag,MPI_COMM_WORLD);
		}
		sendMToNodes();
		for(int j=1;j<numberOfProcesses; j++){
			int currentAnswer;
			MPI_Status incomingSolution;
			MPI_Recv(&currentAnswer,1,MPI_INT,MPI_ANY_SOURCE,sendSolutionTag,MPI_COMM_WORLD,&incomingSolution);
			//solution=new int*[currentAnswer];
			MPI_Status colorList;
			for(int currentColor=0;currentColor<currentAnswer;currentColor++){
				MPI_Probe(incomingSolution.MPI_SOURCE,sendSolutionTag,MPI_COMM_WORLD,&colorList);
				int numberOfVertices;
				MPI_Get_count(&colorList,MPI_INT,&numberOfVertices);
				//solution[currentColor]=new int[numberOfVertices];
				MPI_Recv(&mySolution[currentColor],numberOfVertices,MPI_INT,incomingSolution.MPI_SOURCE,sendSolutionTag,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
				mySolution[currentColor][numberOfVertices]=-1;
				for(int firstVertex=0;firstVertex<numberOfVertices;firstVertex++){
					for(int secondVertex=0;secondVertex<numberOfVertices;secondVertex++){
						if(firstVertex!=secondVertex){
							deltaMemory[firstVertex][secondVertex]+=1.0/currentAnswer;
						}
					}
				}
			}
			if(answer > currentAnswer){
				answer = currentAnswer;
				for(int i=0;i<answer;i++){
					printf("Color %d: ",i+1);
					for(int j=0;mySolution[i][j]>=0;j++){
						printf("%d ",mySolution[i][j]+1);
						finalSolution[mySolution[i][j]]=i+1;
					}
					printf("\n");
				}
				printf("\n");
				//finalSolution=std::move(solution);
			}
			/*for(int k=0;k<currentAnswer;k++){
				printf("Deleting\n");
				delete[] solution[k];
			}
			delete solution;
			*/
		}
	
		for(int vertex1=0; vertex1<vertexNumber; vertex1++)
		{
			for(int vertex2=0; vertex2<vertexNumber; vertex2++)
			{
				if(neighborsScheme[vertex1][vertex2] == 0){
					memory[vertex1][vertex2] = evaporationRate * memory[vertex1][vertex2] + deltaMemory[vertex1][vertex2];
				}
			}		
		}
		
	}
	for(int j=1; j<numberOfProcesses; j++){
		int triggerMessage=-1;
		MPI_Send(&triggerMessage,1,MPI_INT,j,constructSolutionTag,MPI_COMM_WORLD);
	}
	destructor();
	printf("The answer is %d\n",answer);
	output(outputFile,answer,finalSolution);
}

