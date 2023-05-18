#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
//DFS그래프 깊이 우선 탐색

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
typedef struct stackNode
{
	int data;
	struct stackNode* link;
}stackNode;
stackNode* top;
//스택 공백유무
int isEmpty()
{
	if (top == NULL)return TRUE;
	else return FALSE;
}
//푸쉬함수
void push(int item)
{
	stackNode* temp = (stackNode*)malloc(sizeof(stackNode));
	temp->data = item;
	temp->link = top;
	top = temp;
}
//팝함수
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
//그래프g에서 정점 v에 대한 깊이 우선 탐색 연산
void DFS_adjList(graphType* g, int v)
{
	graphNode* w;//새로운 정점 이동
	top == NULL;//스택의 탑 초기화
	push(v);//깊이 우선 탐색을 시작하는 정점 v를 스택에 저장
	g->visited[v] = TRUE;//정점v를 방문했으므로 해당 배열값을 1으로 변경
	printf(" %c", v + 65);
	//스택이 공백이 아닌 동안 깊이 우선 탐색 반복
	while (!isEmpty())
	{
		v = pop();
		w = g->adjList_H[v];
		//인접 정점 w를 방문하지 않은 경우
		while (w)
		{
			//현재 정점 w를 방문하지 않은 경우
			if (!g->visited[w->vertex])
			{
				if (isEmpty())push(v);//정점 v로 다시 돌아올 경우를 위해 다시 저장
				push(w->vertex);//현재 정점w를 스택에 저장
				
				g->visited[w->vertex] = TRUE;//정점 w에 대한 배열값을 1로 설정
				printf(" %c", w->vertex + 65);//정점 0~6를 A~G까지 바꿔출력
				v = w->vertex; //다음 정점으로 이동w->v
				w = g->adjList_H[v];//v에 대한 인접리스트의 첫번째 정점을 w로 생성 
			}
			//현재 정점 w가 이미 방문된 경우
			else
			{
				w = w->link;
			}
		}//while(w) end
	}//스택이 공백이면 깊이 우선 탐색 종료

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

	printf("\n\n////////////////////////\n\n 깊이 우선탐색>> ");
	DFS_adjList(G9, 0);//0번 정점인 A에서  깊이 우선 탐색 시작
}
