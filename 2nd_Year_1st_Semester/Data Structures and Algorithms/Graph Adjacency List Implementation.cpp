//그래프를 인접리스트로 표현하기
#include <stdio.h>
#include <stdlib.h>
#define MAX_VERTEX 30
//인접리스트의 노드 구조
typedef struct GraphNode
{
	int vertex;//정점을 나타내는 데이터 필드
	struct GraphNode* link;//다음 인접 정점을 연결하는 링크필드
}GraphNode;

//그래프를 인접리스트로 표현하기 위한 구조체
typedef struct GraphType
{
	int n;//정점 개수
	GraphNode* adjList_H[ MAX_VERTEX];//정점에 대한 헤드포인터를 저장하는 1차원 배열
};

//공백그래프를 생성하는 연산
void createGraph(GraphType* g)
{
	g->n = 0;//정점의 개수를 0으로 초기화
	for (int v = 0; v < MAX_VERTEX; v++)
	{
		g->adjList_H[v] = NULL;//헤드포인터 배열을 NULL로 초기화
	}
}
//그래프g에 정점v를 삽입하는 연산
void insertVectex(GraphType* g, int v)
{
	if (((g->n) + 1) > MAX_VERTEX)
	{
		printf("\n 그래프의 정점의 개수를 초과했습니다.!");
		return;
	}
	g->n++;

}
//그래프 g에 간선
void insertEdge(GraphType* g, int u, int v)
{
	GraphNode* node;
	if (u >= g->n || v >= g->n)
	{
		printf("\n 그래프에 없는 정접입니다.!");
		return;
	}
	node = (GraphNode*)malloc(sizeof(GraphNode));
	node->vertex = v;
	node->link = g->adjList_H[u];//삽입한 간선에 대한 노드를 리스트의 첫번째 노드로 연결
	g->adjList_H[u] = node;
}

//출력부분
void print_adjList(GraphType* g)
{
	GraphNode* p;
	for (int i = 0; i < g->n; i++)
	{
		printf("\n\t\t정점 %c의 인접리스트",i+65);
		p = g->adjList_H[i];
		while (p)
		{
			printf("->%c", p->vertex+65);//0->A, 1->B,2->C
			p = p->link;
		}
	}
}

void main()
{
	GraphType* G1, * G2, * G3, * G4;
	G1 = (GraphType*)malloc(sizeof(GraphType));
	G2 = (GraphType*)malloc(sizeof(GraphType));
	G3 = (GraphType*)malloc(sizeof(GraphType));
	G4 = (GraphType*)malloc(sizeof(GraphType));

	createGraph(G1);
	createGraph(G2);
	createGraph(G3);
	createGraph(G4);
	
	//그래프 G1
	for (int i = 0; i < 4; i++)
	{
		insertVectex(G1, i);//G1에 정점 0~3 정점 
	}
	insertEdge(G1, 0, 3);
	insertEdge(G1, 0, 1);
	insertEdge(G1, 1, 3);
	insertEdge(G1, 1, 2);
	insertEdge(G1, 1, 0);
	insertEdge(G1, 2, 3);
	insertEdge(G1, 2, 1);
	insertEdge(G1, 3, 2);
	insertEdge(G1, 3, 1);
	insertEdge(G1, 3, 0);
	//그래프 G2
	for (int i = 0; i < 3; i++)
	{
		insertVectex(G2, i);//G2에 정점 0~2 정점 
	}
	insertEdge(G2, 0, 2);
	insertEdge(G2, 0, 1);
	insertEdge(G2, 1, 2);
	insertEdge(G2, 1,0);
	insertEdge(G2, 2, 1);
	insertEdge(G2, 2, 0);
	//그래프 G3
	for (int i = 0; i < 4; i++)
	{
		insertVectex(G3, i);//G3에 정점 0~3 정점 
	}
	insertEdge(G3, 0, 3);
	insertEdge(G3, 0, 1);
	insertEdge(G3, 1, 3);
	insertEdge(G3, 1, 2);
	insertEdge(G3, 2, 3);
	//그래프 G4
	for (int i = 0; i <3; i++)
	{
		insertVectex(G4, i);//G4에 정점 0~3 정점 
	}
	insertEdge(G4, 0, 2);
	insertEdge(G4, 0, 1);
	insertEdge(G4, 1, 2);
	insertEdge(G4, 1, 0);
	//출력
	printf("\nG1의 인접리스트");
	print_adjList(G1);
	printf("\nG2의 인접리스트");
	print_adjList(G2); 
	printf("\nG3의 인접리스트");
	print_adjList(G3);
	printf("\nG4의 인접리스트");
	print_adjList(G4);
}