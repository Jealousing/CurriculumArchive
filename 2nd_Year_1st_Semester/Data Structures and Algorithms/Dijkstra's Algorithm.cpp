#include <stdio.h>
#include <limits.h>

#define TRUE 1
#define FALSE 0
#define MAX_VERTICES 7    //그래프의 정점 개수
#define INF 1000000 //무한대값

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

static int step = 1;//몇번째

int Distance[MAX_VERTICES]; //시작정점으로부터의 최단경로 거리를 표시하는 배열
int Found[MAX_VERTICES]; //방문한 정점을 표시하는 배열

void Print_Status(GraphType* g)//출력
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

int Choose(int distance[], int n, int found[])//최단경로를 갖는 다음 정점을 찾는 연산
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
    Found[Start] = TRUE;//시작 정점을 집합 Distance에 추가
    Distance[Start] = 0; //시작정점의 최단 경로를 0으로설정
    for (int i = 0; i < G->Data - 1; i++)
    {
        Print_Status(G);
        u = Choose(Distance, G->Data, Found);//최단 경로를 만드는 다음 정점 u찾기
        Found[u] = TRUE;

        for (w = 0; w < G->Data; w++)
        {
            if (!Found[w])
            {
                if (Distance[u] + G->Array[u][w] < Distance[w])
                {
                    Distance[w] = Distance[u] + G->Array[u][w];//경로 길이 수정
                }
            }
        }
    }
}
void main(void)
{
    for (int i = 0; i < Q2.Data; i++) //0부터~
    {
        printf("%c시작: \n", i + 65);
        Shortest_Path(&Q2, i);
        step = 1;
    }
}