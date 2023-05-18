#include <stdio.h>
#include <limits.h>

#define MAX_VERTECS 5 //�׷����� ���� ����
#define INF 10000 //���Ѵ�

//�׷���g11�� ����ġ ���� ���
int weight[MAX_VERTECS][MAX_VERTECS] =
{
	{0,10,5,INF,INF},
	{INF,0,2,1,INF},
	{INF,3,0,9,2},
	{INF,INF,INF,0,4},
	{7,INF,INF,6,0}
};
int A[MAX_VERTECS][MAX_VERTECS];//�ִܰ�ι迭
void printStep(int step) {
	int i, j;
	printf("\n A%d : ", step);
	for (i = 0; i < MAX_VERTECS; i++) {
		printf("\t");
		for (j = 0; j < MAX_VERTECS; j++) {
			if (A[i][j] == INF)
				printf("%4c", '*');
			else printf("%4d", A[i][j]);
		}
		printf("\n\n");
	}
}
void Floyd_shortestPath(int n)
{
	int v, w, k, step = -1;
	for (v = 0; v < n; v++)
	{
		for (w = 0; w < n; w++)
		{
			A[v][w] = weight[v][w];
		}
	}
	printStep(step);
	for (k = 0; k < n; k++)
	{
		for (v = 0; v < n; v++)
		{
			for (w = 0; w < n; w++)
			{
				if (A[v][k] + A[k][w] < A[v][w])
					A[v][w] = A[v][k] + A[k][w];
			}
		}
		printStep(++step);
	}
	
}
void main()
{
	printf("\n******����ġ ���� ���******\n\n");
	for (int i = 0; i < MAX_VERTECS; i++)
	{
		for (int j = 0; j < MAX_VERTECS; j++)
		{
			if (weight[i][j] == INF)
				printf("%4c", '*');
			else
				printf("%4d", weight[i][j]);
		}
		printf("\n\n");
	}
	printf("\n****** �÷��̵� �ִ� ��� ���ϱ� ******\n\n");
	Floyd_shortestPath(MAX_VERTECS);
}