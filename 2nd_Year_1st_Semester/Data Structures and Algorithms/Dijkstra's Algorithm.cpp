#include <stdio.h>
#include <limits.h>

#define TRUE 1
#define FALSE 0
#define MAX_VERTICES 7    //�׷����� ���� ����
#define INF 1000000 //���Ѵ밪

typedef struct GraphType {
    int Data;
    int Array[MAX_VERTICES][MAX_VERTICES];
} GraphType;

GraphType Q2 =
{ 7,
   {
     { 0,  7,  INF, INF,   3,  10, INF },
     { 7,  0,    4,  10,   2,   6, INF },
     { INF,  4,    0,   2, INF, INF, INF },
     { INF, 10,    2,   0,  11,   9,   4 },
     { 3,  2,  INF,  11,   0, INF,   5 },
     { 10,  6,  INF,   9, INF,   0, INF },
     { INF, INF, INF,   4,   5, INF,   0 }
   }
};

static int step = 1;//���°

int Distance[MAX_VERTICES]; //�����������κ����� �ִܰ�� �Ÿ��� ǥ���ϴ� �迭
int Found[MAX_VERTICES]; //�湮�� ������ ǥ���ϴ� �迭

void Print_Status(GraphType* g)//���
{
    printf("STEP %d: ", step);
    printf("Distance: ");
    for (int i = 0; i < g->Data; i++)
    {
        if (Distance[i] == INF)
        {
            printf(" * ");
        }
        else
        {
            printf("%2d ", Distance[i]);
        }
    }
    printf("\n        Found:    ");
    for (int i = 0; i < g->Data; i++)
    {
        printf("%2d ", Found[i]);
    }
    printf("\n\n");
    step++;
}

int Choose(int distance[], int n, int found[])//�ִܰ�θ� ���� ���� ������ ã�� ����
{
    int min, minpos;
    min = INT_MAX;//limits.h
    minpos = -1;
    for (int i = 0; i < n; i++)
    {
        if (distance[i] < min && !found[i])
        {
            min = distance[i];
            minpos = i;
        }
    }
    return minpos;
}

void Shortest_Path(GraphType* G, int Start)
{
    int  u, w;
    for (int i = 0; i < G->Data; i++)
    {
        Distance[i] = G->Array[Start][i];
        Found[i] = FALSE;
    }
    Found[Start] = TRUE;//���� ������ ���� Distance�� �߰�
    Distance[Start] = 0; //���������� �ִ� ��θ� 0���μ���
    for (int i = 0; i < G->Data - 1; i++)
    {
        Print_Status(G);
        u = Choose(Distance, G->Data, Found);//�ִ� ��θ� ����� ���� ���� uã��
        Found[u] = TRUE;

        for (w = 0; w < G->Data; w++)
        {
            if (!Found[w])
            {
                if (Distance[u] + G->Array[u][w] < Distance[w])
                {
                    Distance[w] = Distance[u] + G->Array[u][w];//��� ���� ����
                }
            }
        }
    }
}
void main(void)
{
    for (int i = 0; i < Q2.Data; i++) //0����~
    {
        printf("%c����: \n", i + 65);
        Shortest_Path(&Q2, i);
        step = 1;
    }
}