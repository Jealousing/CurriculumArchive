#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
//BFS 그래프 너비 우선 탐색

#define MAX_VERTEX 10
#define FALSE 0
#define TRUE 1
//그래프에 대한 인접리스트 노드 구조 정의

typedef struct graphNode
{
	int vertex;
	struct graphNode* link;
}graphNode;

typedef struct graphType
{
	int n;//정점개수
	graphNode* adjList_H[MAX_VERTEX];//인접리스트의 헤드 노드 배열
	int visited[MAX_VERTEX];//방문표시를 위한 배열
};
//연결리스트를 사용한 스택
typedef int element;

// << 큐 (6장에서 설명한 연결 리스트를 이용한 큐 연산 수행:[예제 6-3]의 05~60행과 동일)
typedef struct QNode {
    int data;
    struct QNode* link;
} QNode;

typedef struct {
    QNode* front, * rear;
} LQueueType;

LQueueType* createLinkedQueue() {
    LQueueType* LQ;
    LQ = (LQueueType*)malloc(sizeof(LQueueType));
    LQ->front = NULL;
    LQ->rear = NULL;
    return LQ;
}

int isEmpty(LQueueType* LQ) {
    if (LQ->front == NULL) {
        printf("\n Linked Queue is empty! \n");
        return 1;
    }
    else return 0;
}

void enQueue(LQueueType* LQ, int item) {
    QNode* newNode = (QNode*)malloc(sizeof(QNode));
    newNode->data = item;
    newNode->link = NULL;
    if (LQ->front == NULL) {
        LQ->front = newNode;
        LQ->rear = newNode;
    }
    else {
        LQ->rear->link = newNode;
        LQ->rear = newNode;
    }
}

int deQueue(LQueueType* LQ) {
    QNode* old = LQ->front;
    int item;
    if (isEmpty(LQ)) return 0;
    else {
        item = old->data;
        LQ->front = LQ->front->link;
        if (LQ->front == NULL)
            LQ->rear = NULL;
        free(old);
        return item;
    }
} // 큐 >>

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

//그래프g에 정점 v를 삽입하는 연산
void insertVertex(graphType* g, int v)
{
    if (((g->n) + 1) > MAX_VERTEX)
    {
        printf("\n 그래프의 정점의 개수를 초과했습니다.!");
        return;
    }
    g->n++;
}
// 그래프 g에 간선 (u, v)를 삽입하는 연산 : [예제 8-2]의 35~47행과 동일
void insertEdge(graphType* g, int u, int v) {
    graphNode* node;
    if (u >= g->n || v >= g->n) {
        printf("\n 그래프에 없는 정점입니다!");
        return;
    }
    node = (graphNode*)malloc(sizeof(graphNode));
    node->vertex = v;
    node->link = g->adjList_H[u];
    g->adjList_H[u] = node;
}
// 그래프 g의 각 정점에 대한 인접 리스트를 출력하는 연산 : [예제 8-2]의 50~61행과 동일
void print_adjList(graphType* g)
{
    int i;
    graphNode* p;
    for (i = 0; i < g->n; i++) {
        printf("\n\t\t정점%c의 인접 리스트", i + 65);
        p = g->adjList_H[i];
        while (p) {
            printf(" -> %c", p->vertex + 65);
            p = p->link;
        }
    }
}
//그래프 G에서 정점V에 대한 너비 우선 탐색 연산
void BFS_adjList(graphType* g, int v)
{
    graphNode* w;
    LQueueType* Q;
    Q = createLinkedQueue();
    g->visited[v] = TRUE;
    printf(" %c", v + 65);
    enQueue(Q, v);
    //큐가 공백인 아닌 동안 너비우선 탐색 수행
    while (!isEmpty(Q))
    {
        v = deQueue(Q);
        //현재 정점 w를 방문하지 않은 경우
        for (w = g->adjList_H[v]; w; w = w->link)
        {
            if (!g->visited[w->vertex])
            {
                g->visited[w->vertex] = TRUE;
                printf(" %c", w->vertex + 65);
                enQueue(Q, w->vertex);
            }
        }
    }
}

void main()
{
    graphType* G9;
    G9 = (graphType*)malloc(sizeof(graphType));
    createGraph(G9);
    //그래프 G9 구성
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

    printf("G9의 인접 리스트");
    print_adjList(G9);

    printf("\n\n////////////////////////\n\n 너비 우선탐색>> ");
    BFS_adjList(G9, 0);//0번 정점인 A에서  깊이 우선 탐색 시작
}