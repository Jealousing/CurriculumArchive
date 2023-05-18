#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
//DFS�׷��� ���� �켱 Ž��

#define MAX_VERTEX 10
#define FALSE 0
#define TRUE 1
//�׷����� ���� ��������Ʈ ��� ���� ����

typedef struct graphNode
{
	int vertex;
	struct graphNode* link;
}graphNode;

typedef struct graphType
{
	int n;//��������
	graphNode* adjList_H[MAX_VERTEX];//��������Ʈ�� ��� ��� �迭
	int visited[MAX_VERTEX];//�湮ǥ�ø� ���� �迭
};
//���Ḯ��Ʈ�� ����� ����
typedef int element;
typedef struct stackNode
{
	int data;
	struct stackNode* link;
}stackNode;
stackNode* top;
//���� ��������
int isEmpty()
{
	if (top == NULL)return TRUE;
	else return FALSE;
}
//Ǫ���Լ�
void push(int item)
{
	stackNode* temp = (stackNode*)malloc(sizeof(stackNode));
	temp->data = item;
	temp->link = top;
	top = temp;
}
//���Լ�
int pop()
{
	int item;
	stackNode* temp = top;
	if (isEmpty())
	{
		printf("\n\n STACK IS EMPTY!!! \n");
		return FALSE;
	}
	else
	{
		item = temp->data;
		top = temp->link;
		free(temp);
		return item;
	}
}
void createGraph(graphType* g)
{
	int v;
	g->n = 0;
	for (v = 0; v < MAX_VERTEX; v++)
	{
		g->visited[v] = FALSE;
		g->adjList_H[v] = NULL;
	}
}

//�׷���g�� ���� v�� �����ϴ� ����
void insertVertex(graphType* g, int v)
{
	if (((g->n) + 1) > MAX_VERTEX)
	{
		printf("\n �׷����� ������ ������ �ʰ��߽��ϴ�.!");
		return;
	}
	g->n++;
}
// �׷��� g�� ���� (u, v)�� �����ϴ� ���� : [���� 8-2]�� 35~47��� ����
void insertEdge(graphType* g, int u, int v) {
	graphNode* node;
	if (u >= g->n || v >= g->n) {
		printf("\n �׷����� ���� �����Դϴ�!");
		return;
	}
	node = (graphNode*)malloc(sizeof(graphNode));
	node->vertex = v;
	node->link = g->adjList_H[u];
	g->adjList_H[u] = node;
}
// �׷��� g�� �� ������ ���� ���� ����Ʈ�� ����ϴ� ���� : [���� 8-2]�� 50~61��� ����
void print_adjList(graphType* g) 
{
	int i;
	graphNode* p;
	for (i = 0; i < g->n; i++) {
		printf("\n\t\t����%c�� ���� ����Ʈ", i + 65);
		p = g->adjList_H[i];
		while (p) {
			printf(" -> %c", p->vertex + 65);
			p = p->link;
		}
	}
}
//�׷���g���� ���� v�� ���� ���� �켱 Ž�� ����
void DFS_adjList(graphType* g, int v)
{
	graphNode* w;//���ο� ���� �̵�
	top == NULL;//������ ž �ʱ�ȭ
	push(v);//���� �켱 Ž���� �����ϴ� ���� v�� ���ÿ� ����
	g->visited[v] = TRUE;//����v�� �湮�����Ƿ� �ش� �迭���� 1���� ����
	printf(" %c", v + 65);
	//������ ������ �ƴ� ���� ���� �켱 Ž�� �ݺ�
	while (!isEmpty())
	{
		v = pop();
		w = g->adjList_H[v];
		//���� ���� w�� �湮���� ���� ���
		while (w)
		{
			//���� ���� w�� �湮���� ���� ���
			if (!g->visited[w->vertex])
			{
				if (isEmpty())push(v);//���� v�� �ٽ� ���ƿ� ��츦 ���� �ٽ� ����
				push(w->vertex);//���� ����w�� ���ÿ� ����
				
				g->visited[w->vertex] = TRUE;//���� w�� ���� �迭���� 1�� ����
				printf(" %c", w->vertex + 65);//���� 0~6�� A~G���� �ٲ����
				v = w->vertex; //���� �������� �̵�w->v
				w = g->adjList_H[v];//v�� ���� ��������Ʈ�� ù��° ������ w�� ���� 
			}
			//���� ���� w�� �̹� �湮�� ���
			else
			{
				w = w->link;
			}
		}//while(w) end
	}//������ �����̸� ���� �켱 Ž�� ����

}
void main()
{
	graphType* G9;
	G9 = (graphType*)malloc(sizeof(graphType));
	createGraph(G9);
	//�׷��� G9 ����
	for (int i = 0; i < 7; i++)
	{
		insertVertex(G9, i);
	}
	insertEdge(G9, 0, 2);
	insertEdge(G9, 0, 1);
	insertEdge(G9, 1, 4);
	insertEdge(G9, 1, 3);
	insertEdge(G9, 1, 0);
	insertEdge(G9, 2, 4);
	insertEdge(G9, 2, 0);
	insertEdge(G9, 3, 6);
	insertEdge(G9, 3, 1);
	insertEdge(G9, 4, 6);
	insertEdge(G9, 4, 2);
	insertEdge(G9, 4, 1);
	insertEdge(G9, 5, 6);
	insertEdge(G9, 6, 5);
	insertEdge(G9, 6, 4);
	insertEdge(G9, 6, 3);
	
	printf("G9�� ���� ����Ʈ");
	print_adjList(G9);

	printf("\n\n////////////////////////\n\n ���� �켱Ž��>> ");
	DFS_adjList(G9, 0);//0�� ������ A����  ���� �켱 Ž�� ����
}
