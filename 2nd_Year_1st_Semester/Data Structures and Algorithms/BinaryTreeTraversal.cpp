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
        Left_Size = calc(root->Left);//후위
        Right_Size = calc(root->Right);//후위
        return Left_Size + Right_Size + root->Data;//계산한 용량값을 리턴한다.
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

    printf("C:\\의 용량은 %d MB입니다.\n\n", calc(&F2));

    printf("D:\\의 용량은 %d MB입니다.\n\n", calc(&F3));
    
    printf("내 컴퓨터의 전체 용량은 %d MB입니다.\n\n", calc(&F1));
}
