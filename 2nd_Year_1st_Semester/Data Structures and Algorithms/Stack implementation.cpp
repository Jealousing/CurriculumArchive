#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE 100//������ ũ��

char stack[STACK_SIZE];//1���� �迭�� ���ü���
int top = -1;//top �ʱ�ȭ

int isEmpty()//������ ������ Ȯ���ϴ� �Լ�
{
	if (top == -1)
	{
		return true;
	}
	return false;
}

int isFull()//������ ��ȭ�������� Ȯ���ϴ� �Լ�
{
	if (top == STACK_SIZE - 1)
	{
		return 1;
	}
	else
		return 0;
}

void Push(char item)//���ÿ� �����͸� Ǫ���ϴ� �Լ�
{
	if (isFull())
	{
		printf("\n���ò���");
	}
	else
	{
		stack[++top] = item;
	}
}

char Pop()//���ÿ��� �����͸� ��(����)�ϴ� �Լ�
{
	if (isEmpty())
	{
		printf("\n���þ���");
	}
	else
	{
		return stack[top--];
	}
}

void printStack()//���ÿ� ���� ���Ҹ� ����ϴ� ����
{
	printf("\nSTACK[");
	for (int i = 0; i <= top; i++)
	{
		printf(" %c ", stack[i]);
	}
	printf("]\n");
}

//�������� �����Լ�
int  main()
{
	char item[50];//���ÿ� �����ϴ� ����
	char str;
	printf("\n5�ڸ� ���ڿ��� �Է��ϼ��� : "); 
	gets_s(item);//�Է¹ޱ�

	//�Է¹��� ���ڿ��� ���ÿ� ����
	for (int i = 0; i < 5; i++)
	{
		Push(item[i]);
	}
	printStack(); //�ѹ� ���

	printf("\n���� ���ڿ�: "); //�̾Ƴ��鼭 ���
	for (int i = 0; i < 5; i++)
	{
		str =  Pop(); //������ ���� ���ں��� �������� ���
		printf("%c",str);
	}
	
	return 0;
}
