//�׷����� ������ķ� ǥ���ϱ�
#include <stdio.h>
#include <stdlib.h>
#define MAX_VERTEX 30

//�׷����� ������ķ� ǥ���ϱ� ���� ����ü
typedef struct GraphType
{
	int n;//��������
	int adjMatrix[MAX_VERTEX][MAX_VERTEX];//30*30 2�����迭
}GraphType;

void createGraph(GraphType* g)
{
	g->n = 0;//���� ������ 0���� �ʱ�ȭ
	for (int i = 0; i < MAX_VERTEX; i++)
	{
		for (int j = 0; j < MAX_VERTEX; j++)
		{
			g->adjMatrix[i][j] = 0; //2�����迭�� 0���� �ʱ�ȭ
		}
	}
}
//�׷���g�� ���� v�� �����ϴ� ����
void insertVertex(GraphType* g,int v)
{
	if (((g->n) + 1) > MAX_VERTEX)
	{
		printf("\n �׷����� ������ ������ �ʰ��߽��ϴ�.!");
		return;
	}
	g->n++;
}
//�׷��� g�� ���� (u,v)�� �����ϴ� ����
void insertEdge(GraphType* g, int u, int v)
{
	if (u >= g->n||v>=g->n)
	{
		printf("\n �׷����� ���� �����Դϴ�.!");
		return;
	}
	g->adjMatrix[u][v] = 1;//������ ������ ���� 2�����迭�� ���Ұ��� 1�� ����
}
//�׷�����g�� 2�����迭�� ������� ����ϴ� ����
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

	createGraph(G1);//4*4 ����迭
	createGraph(G2);//3*3 ����迭
	createGraph(G3);//4*4 ����迭
	createGraph(G4);//3*3 ����迭

	//G1�׷��� ����
	for (int i = 0; i < 4; i++)
	{
		insertVertex(G1, i);//G1�� ������ 0~3���� ����
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
	//G2�׷��� ����
	for (int i = 0; i < 3; i++)
	{
		insertVertex(G2, i);//G1�� ������ 0~2���� ����
	}
	insertEdge(G2, 0, 1);
	insertEdge(G2, 0, 2);
	insertEdge(G2, 1, 0);
	insertEdge(G2, 1, 2);
	insertEdge(G2, 2, 0);
	insertEdge(G2, 2, 1);
	//G3�׷��� ����
	for (int i = 0; i < 4; i++)
	{
		insertVertex(G3, i);//G1�� ������ 0~3���� ����
	}
	insertEdge(G3, 0, 1);
	insertEdge(G3, 0, 3);
	insertEdge(G3, 1, 2);
	insertEdge(G3, 1, 3);
	insertEdge(G3, 2, 3);
	//G4�׷��� ����
	for (int i = 0; i < 3; i++)
	{
		insertVertex(G4, i);//G1�� ������ 0~2���� ����
	}
	insertEdge(G4, 0, 1);
	insertEdge(G4, 0, 2);
	insertEdge(G4, 1, 0);
	insertEdge(G4, 1, 2);

	printf("\n G1�� �������");
	print_adjMatrix(G1);
	printf("\n G2�� �������");
	print_adjMatrix(G2);
	printf("\n G3�� �������");
	print_adjMatrix(G3);
	printf("\n G4�� �������");
	print_adjMatrix(G4);
	return 0;
}