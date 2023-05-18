//�׷����� ��������Ʈ�� ǥ���ϱ�
#include <stdio.h>
#include <stdlib.h>
#define MAX_VERTEX 30
//��������Ʈ�� ��� ����
typedef struct GraphNode
{
	int vertex;//������ ��Ÿ���� ������ �ʵ�
	struct GraphNode* link;//���� ���� ������ �����ϴ� ��ũ�ʵ�
}GraphNode;

//�׷����� ��������Ʈ�� ǥ���ϱ� ���� ����ü
typedef struct GraphType
{
	int n;//���� ����
	GraphNode* adjList_H[ MAX_VERTEX];//������ ���� ��������͸� �����ϴ� 1���� �迭
};

//����׷����� �����ϴ� ����
void createGraph(GraphType* g)
{
	g->n = 0;//������ ������ 0���� �ʱ�ȭ
	for (int v = 0; v < MAX_VERTEX; v++)
	{
		g->adjList_H[v] = NULL;//��������� �迭�� NULL�� �ʱ�ȭ
	}
}
//�׷���g�� ����v�� �����ϴ� ����
void insertVectex(GraphType* g, int v)
{
	if (((g->n) + 1) > MAX_VERTEX)
	{
		printf("\n �׷����� ������ ������ �ʰ��߽��ϴ�.!");
		return;
	}
	g->n++;

}
//�׷��� g�� ����
void insertEdge(GraphType* g, int u, int v)
{
	GraphNode* node;
	if (u >= g->n || v >= g->n)
	{
		printf("\n �׷����� ���� �����Դϴ�.!");
		return;
	}
	node = (GraphNode*)malloc(sizeof(GraphNode));
	node->vertex = v;
	node->link = g->adjList_H[u];//������ ������ ���� ��带 ����Ʈ�� ù��° ���� ����
	g->adjList_H[u] = node;
}

//��ºκ�
void print_adjList(GraphType* g)
{
	GraphNode* p;
	for (int i = 0; i < g->n; i++)
	{
		printf("\n\t\t���� %c�� ��������Ʈ",i+65);
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
	
	//�׷��� G1
	for (int i = 0; i < 4; i++)
	{
		insertVectex(G1, i);//G1�� ���� 0~3 ���� 
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
	//�׷��� G2
	for (int i = 0; i < 3; i++)
	{
		insertVectex(G2, i);//G2�� ���� 0~2 ���� 
	}
	insertEdge(G2, 0, 2);
	insertEdge(G2, 0, 1);
	insertEdge(G2, 1, 2);
	insertEdge(G2, 1,0);
	insertEdge(G2, 2, 1);
	insertEdge(G2, 2, 0);
	//�׷��� G3
	for (int i = 0; i < 4; i++)
	{
		insertVectex(G3, i);//G3�� ���� 0~3 ���� 
	}
	insertEdge(G3, 0, 3);
	insertEdge(G3, 0, 1);
	insertEdge(G3, 1, 3);
	insertEdge(G3, 1, 2);
	insertEdge(G3, 2, 3);
	//�׷��� G4
	for (int i = 0; i <3; i++)
	{
		insertVectex(G4, i);//G4�� ���� 0~3 ���� 
	}
	insertEdge(G4, 0, 2);
	insertEdge(G4, 0, 1);
	insertEdge(G4, 1, 2);
	insertEdge(G4, 1, 0);
	//���
	printf("\nG1�� ��������Ʈ");
	print_adjList(G1);
	printf("\nG2�� ��������Ʈ");
	print_adjList(G2); 
	printf("\nG3�� ��������Ʈ");
	print_adjList(G3);
	printf("\nG4�� ��������Ʈ");
	print_adjList(G4);
}