#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct TreeNode
{
    int Data;
    struct TreeNode* Left, * Right;
}treeNode;

int calc(TreeNode* root)
{
    int Left_Size, Right_Size;
    if (root == NULL)return 0;
    else
    {
        Left_Size = calc(root->Left);//����
        Right_Size = calc(root->Right);//����
        return Left_Size + Right_Size + root->Data;//����� �뷮���� �����Ѵ�.
    }
}
int main()
{
    TreeNode F11 = { 120,NULL,NULL };
    TreeNode F10 = { 55,NULL,NULL };
    TreeNode F9 = { 100,NULL,NULL };
    TreeNode F8 = { 200,NULL,NULL };
    TreeNode F7 = { 68,&F10,&F11 };
    TreeNode F6 = { 40,NULL,NULL };
    TreeNode F5 = { 15,NULL,NULL };
    TreeNode F4 = { 2,&F8,&F9 };
    TreeNode F3 = { 10,&F6,&F7 };
    TreeNode F2 = { 0,&F4,&F5 };
    TreeNode F1 = { 0,&F2,&F3 };

    printf("C:\\�� �뷮�� %d MB�Դϴ�.\n\n", calc(&F2));

    printf("D:\\�� �뷮�� %d MB�Դϴ�.\n\n", calc(&F3));
    
    printf("�� ��ǻ���� ��ü �뷮�� %d MB�Դϴ�.\n\n", calc(&F1));
}
