#include <stdio.h>
#include <vector>
#include <algorithm>

using namespace std;

int check[7];	//노드 연결용, 연결노드가 바뀌는지 체크 

class Edge {
public:
	int node[2];
	int distance;
	Edge(int a, int b, int distance) {
		this->node[0] = a;
		this->node[1] = b;
		this->distance = distance;
	}

	//간선을 오름차순으로 정렬할때 기준을 distance로 정해줍니다. 
	bool operator<(Edge& edge) {
		return this->distance < edge.distance;
	}
};

int getParent(int node) {
	if (check[node] == node) return node;
	return getParent(check[node]);
}

//두 노드를 작은값을 기준으로 연결합니다. 
void unionParent(int node1, int node2) {
	node1 = getParent(node1);
	node2 = getParent(node2);
	if (node1 < node2) check[node2] = node1;
	else check[node1] = node2;
}

//싸이클이 존재하면 true, 아니면 false를 반환
bool isCycle(int node1, int node2) {
	node1 = getParent(node1);
	node2 = getParent(node2);
	if (node1 == node2) return true;
	else return false;
}

int main() {
	//두 노드를 연결할 간선을 정해줍니다. 
	vector<Edge> v;
	//a1 b2 c3 d4 e5 f6
	v.push_back(Edge(1, 2, 8));
	v.push_back(Edge(1, 4, 2));
	v.push_back(Edge(1, 5, 4));
	v.push_back(Edge(2, 3, 1));
	v.push_back(Edge(2, 4, 4));
	v.push_back(Edge(2, 6, 2));
	v.push_back(Edge(3, 2, 1));
	v.push_back(Edge(3, 6, 1));
	v.push_back(Edge(4, 5, 3));
	v.push_back(Edge(4, 6, 7));
	v.push_back(Edge(5, 6, 9));

	//간선을 오름차순으로 정렬합니다. 
	sort(v.begin(), v.end());

	//각 노드는 자기자신이 부모로 초기화해줍니다. 
	for (int i = 1; i <= 7; ++i) {
		check[i] = i;
	}

	int sum = 0;
	for (int i = 0; i < v.size(); ++i) {
		//싸이클이 존재하지 않으면 비용을 더합니다. 
		if (!isCycle(v[i].node[0], v[i].node[1])) {
			sum += v[i].distance;
			unionParent(v[i].node[0], v[i].node[1]);
		}
	}

	printf("%d\n", sum);

	return 0;
}