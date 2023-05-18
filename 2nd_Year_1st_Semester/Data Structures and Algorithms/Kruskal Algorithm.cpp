#include <stdio.h>
#include <vector>
#include <algorithm>

using namespace std;

int check[7];	//��� �����, �����尡 �ٲ���� üũ 

class Edge {
public:
	int node[2];
	int distance;
	Edge(int a, int b, int distance) {
		this->node[0] = a;
		this->node[1] = b;
		this->distance = distance;
	}

	//������ ������������ �����Ҷ� ������ distance�� �����ݴϴ�. 
	bool operator<(Edge& edge) {
		return this->distance < edge.distance;
	}
};

int getParent(int node) {
	if (check[node] == node) return node;
	return getParent(check[node]);
}

//�� ��带 �������� �������� �����մϴ�. 
void unionParent(int node1, int node2) {
	node1 = getParent(node1);
	node2 = getParent(node2);
	if (node1 < node2) check[node2] = node1;
	else check[node1] = node2;
}

//����Ŭ�� �����ϸ� true, �ƴϸ� false�� ��ȯ
bool isCycle(int node1, int node2) {
	node1 = getParent(node1);
	node2 = getParent(node2);
	if (node1 == node2) return true;
	else return false;
}

int main() {
	//�� ��带 ������ ������ �����ݴϴ�. 
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

	//������ ������������ �����մϴ�. 
	sort(v.begin(), v.end());

	//�� ���� �ڱ��ڽ��� �θ�� �ʱ�ȭ���ݴϴ�. 
	for (int i = 1; i <= 7; ++i) {
		check[i] = i;
	}

	int sum = 0;
	for (int i = 0; i < v.size(); ++i) {
		//����Ŭ�� �������� ������ ����� ���մϴ�. 
		if (!isCycle(v[i].node[0], v[i].node[1])) {
			sum += v[i].distance;
			unionParent(v[i].node[0], v[i].node[1]);
		}
	}

	printf("%d\n", sum);

	return 0;
}