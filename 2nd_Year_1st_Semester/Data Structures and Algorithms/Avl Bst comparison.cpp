#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
//책에 없는 트리 AVL트리 
//L < P(부모노드) < R  균형인수{-1,0,+1}
#define MAX(a,b) ((a>b)?a:b)
typedef int element;
typedef struct treeNode
{
	element key;//노드에 저장된 값
	struct  treeNode* left;//자기참조구조체 왼쪽노드
	struct treeNode* right;//자기참조구조체 오른쪽노드
}treeNode;

//LL회전 L1 : parent, L2 : Child
treeNode* LL_rotate(treeNode* parent)
{
	treeNode* child = parent->left;
	parent->left = child->right;//L2->LL1
	child->right = parent;
	return child;
}
//RR회전
treeNode* RR_rotate(treeNode* parent)
{
	treeNode* child = parent->right;
	parent->right = child->left;
	child->left = parent;
	return child;
}
//LR회전  RR->LL
treeNode* LR_rotate(treeNode* parent)
{
	treeNode* child = parent->left;
	parent->left = RR_rotate(child);//RR
	return LL_rotate(parent);//LL
}
//RL회전 LL->RR
treeNode* RL_rotate(treeNode* parent)
{
	treeNode* child = parent->right;
	parent->right = LL_rotate(child);
	return RR_rotate(parent);
}
//서브트리의 높이를 구하는 연산
int getHeight(treeNode* p)
{
	int height = 0;
	if (p != NULL) height = MAX(getHeight(p->left), getHeight(p->right))+1;
	return height;
}

//서브트리의 높이를 이용해 균형인수를 구하는연산
int getBF(treeNode* p)
{
	if (p == NULL)return 0;
	return getHeight(p->left) - getHeight(p->right); //왼쪽 트리 높이 - 오른쪽트리 높이
}
//BF를 검사하여 불균형ㅇ ㅣ발생한 경우, 회산 연산 호출
treeNode* rebalance(treeNode** p)
{
	int BF = getBF(*p);
	if (BF > 1) {
		if (getBF((*p)->left) > 0)
			*p = LL_rotate(*p);
		else    *p = LR_rotate(*p);
	}
	else if (BF < -1) {
		if (getBF((*p)->right) < 0)
			*p = RR_rotate(*p);
		else *p = RL_rotate(*p);
	}
	return *p;
}
//AVL 트리에 노드를 삽입하는 연산
treeNode* insert_AVL_Node(treeNode** root, element x)
{
	if (*root == NULL)
	{
		*root = (treeNode*)malloc(sizeof(treeNode));
		(*root)->key = x;
		(*root)->left = NULL;
		(*root)->right = NULL;
	}
	else if (x < (*root)->key)
	{
		(*root)->left = insert_AVL_Node(&((*root)->left), x);
		*root = rebalance(root);
	}
	else if (x > (*root)->key)
	{
		(*root)->right =insert_AVL_Node( & ((*root)->right), x);
		*root = rebalance(root);
	}
	else
	{
		printf("\n이미 같은 키가 있습니다.!\n");
		exit(1);
	}
	return *root;
}
//이진 탐색 트리에 노드를 삽입하는 연산
treeNode* insert_BST_Node(treeNode* p, element x) {
	treeNode* newNode;
	if (p == NULL) {
		newNode = (treeNode*)malloc(sizeof(treeNode));
		newNode->key = x;
		newNode->left = NULL;
		newNode->right = NULL;
		return newNode;
	}
	else if (x < p->key)  p->left = insert_BST_Node(p->left, x);
	else if (x > p->key)  p->right = insert_BST_Node(p->right, x);
	else  printf("\n 이미 같은 키가 있습니다! \n");

	return p;
}
void displayInorder(treeNode* root)
{
	if (root)
	{
		displayInorder(root->left);
		printf("_%d", root->key);
		displayInorder(root->right);
	}
}
//AVL트리에서 키값을 탐색하는 연산
treeNode* searchTree(treeNode* root, element x) {
	treeNode* p;
	int count = 0;
	p = root;
	while (p != NULL) {
		count++;
		if (x < p->key) p = p->left;
		else if (x == p->key) {
			printf("%3d번째에 탐색 성공", count);
			return p;
		}
		else p = p->right;
	}
	count++;
	printf("%3d번째에 탐색 실패! 찾는 키가 없습니다!", count);
	return p;
}
int main()
{
	treeNode* root_AVL = NULL;//균형이진탐색트리 포인터
	treeNode* root_BST = NULL;//이진탐색트리 포인터
	
    //균형 이진탐색트리 초기화(삽입)
	//50,60,70,90,80,75,73,72,78
	root_AVL = insert_AVL_Node(&root_AVL, 50);
	insert_AVL_Node(&root_AVL, 60);
	insert_AVL_Node(&root_AVL, 70);
	insert_AVL_Node(&root_AVL, 90);
	insert_AVL_Node(&root_AVL, 80);
	insert_AVL_Node(&root_AVL, 75);
	insert_AVL_Node(&root_AVL, 73);
	insert_AVL_Node(&root_AVL, 72);
	insert_AVL_Node(&root_AVL, 78);

	printf("\n ************AVL 트리 출력 ************\n\n");
	displayInorder(root_AVL);
	printf("\n\n AVL트리에서 70 탐색 : ");
	searchTree(root_AVL, 70);
	printf("\n\n AVL트리에서 72 탐색 : ");
	searchTree(root_AVL, 72);
	printf("\n\n AVL트리에서 76 탐색 : ");
	searchTree(root_AVL, 76);
	//이진탐색트리 초기화 (삽입)
	root_BST = insert_BST_Node(root_BST, 50);
	insert_BST_Node(root_BST, 60);
	insert_BST_Node(root_BST, 70);
	insert_BST_Node(root_BST, 90);
	insert_BST_Node(root_BST, 80);
	insert_BST_Node(root_BST, 75);
	insert_BST_Node(root_BST, 73);
	insert_BST_Node(root_BST, 72);
	insert_BST_Node(root_BST, 78);

	printf("\n ************BST 트리 출력 ************\n\n");
	displayInorder(root_BST);
	printf("\n\n BST트리에서 70 탐색 : ");
	searchTree(root_BST, 70);
	printf("\n\n BST트리에서 72 탐색 : ");
	searchTree(root_BST, 72);
	printf("\n\n BST트리에서 76 탐색 : ");
	searchTree(root_BST, 76);
}