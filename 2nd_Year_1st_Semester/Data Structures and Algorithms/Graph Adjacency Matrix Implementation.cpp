//그래프를 인접행렬로 표현하기
#include <stdio.h>
#include <stdlib.h>
#define MAX_VERTEX 30

//그래프를 인접행렬로 표현하기 위해 구조체
typedef struct GraphType
{
	int n;//정점개수
	int adjMatrix[MAX_VERTEX][MAX_VERTEX];//30*30 2차원배열
}GraphType;

void createGraph(GraphType* g)
{
	g->n = 0;//정점 개수를 0으로 초기화
	for (int i = 0; i < MAX_VERTEX; i++)
	{
		for (int j = 0; j < MAX_VERTEX; j++)
		{
			g->adjMatrix[i][j] = 0; //2차원배열을 0으로 초기화
		}
	}
}
//그래프g에 정점 v를 삽입하는 연산
void insertVertex(GraphType* g,int v)
{
	if (((g->n) + 1) > MAX_VERTEX)
	{
		printf("\n 그래프의 정점의 개수를 초과했습니다.!");
		return;
	}
	g->n++;
}
//그래프 g에 간선 (u,v)를 삽입하는 연산
void insertEdge(GraphType* g, int u, int v)
{
	if (u >= g->n||v>=g->n)
	{
		printf("\n 그래프에 없는 정접입니다.!");
		return;
	}
	g->adjMatrix[u][v] = 1;//삽인한 간선에 대한 2차원배열의 원소값을 1로 설정
}
//그래프의g의 2차원배열을 순서대로 출력하는 연산
void print_adjMatrix(GraphType* g)
{
	for (int i = 0; i < (g->n); i++)
	{
		printf("\n\t\t");
		for (int j = 0; j < (g->n); j++)
		{
			printf("%2d", g->adjMatrix[i][j]);
		}
	}
}

int main()
{
	GraphType* G1, *G2, *G3, *G4;
	G1 = (GraphType*)malloc(sizeof(GraphType));
	G2 = (GraphType*)malloc(sizeof(GraphType));
	G3 = (GraphType*)malloc(sizeof(GraphType));
	G4 = (GraphType*)malloc(sizeof(GraphType));

	createGraph(G1);//4*4 공백배열
	createGraph(G2);//3*3 공백배열
	createGraph(G3);//4*4 공백배열
	createGraph(G4);//3*3 공백배열

	//G1그래프 설정
	for (int i = 0; i < 4; i++)
	{
		insertVertex(G1, i);//G1의 정점을 0~3으로 삽입
	}
	insertEdge(G1, 0, 1);
	insertEdge(G1, 0, 3);
	insertEdge(G1, 1, 0);
	insertEdge(G1, 1, 2);
	insertEdge(G1, 1, 3);
	insertEdge(G1, 2, 1);
	insertEdge(G1, 2, 3);
	insertEdge(G1, 3, 0);
	insertEdge(G1, 3, 1);
	insertEdge(G1, 3, 2);
	//G2그래프 설정
	for (int i = 0; i < 3; i++)
	{
		insertVertex(G2, i);//G1의 정점을 0~2으로 삽입
	}
	insertEdge(G2, 0, 1);
	insertEdge(G2, 0, 2);
	insertEdge(G2, 1, 0);
	insertEdge(G2, 1, 2);
	insertEdge(G2, 2, 0);
	insertEdge(G2, 2, 1);
	//G3그래프 설정
	for (int i = 0; i < 4; i++)
	{
		insertVertex(G3, i);//G1의 정점을 0~3으로 삽입
	}
	insertEdge(G3, 0, 1);
	insertEdge(G3, 0, 3);
	insertEdge(G3, 1, 2);
	insertEdge(G3, 1, 3);
	insertEdge(G3, 2, 3);
	//G4그래프 설정
	for (int i = 0; i < 3; i++)
	{
		insertVertex(G4, i);//G1의 정점을 0~2으로 삽입
	}
	insertEdge(G4, 0, 1);
	insertEdge(G4, 0, 2);
	insertEdge(G4, 1, 0);
	insertEdge(G4, 1, 2);

	printf("\n G1의 인접행렬");
	print_adjMatrix(G1);
	printf("\n G2의 인접행렬");
	print_adjMatrix(G2);
	printf("\n G3의 인접행렬");
	print_adjMatrix(G3);
	printf("\n G4의 인접행렬");
	print_adjMatrix(G4);
	return 0;
}