#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE 100//스택의 크기

char stack[STACK_SIZE];//1차원 배열로 스택선언
int top = -1;//top 초기화

int isEmpty()//스택의 공백을 확인하는 함수
{
	if (top == -1)
	{
		return true;
	}
	return false;
}

int isFull()//스택이 포화상태인지 확인하는 함수
{
	if (top == STACK_SIZE - 1)
	{
		return 1;
	}
	else
		return 0;
}

void Push(char item)//스택에 데이터를 푸시하는 함수
{
	if (isFull())
	{
		printf("\n스택꽉참");
	}
	else
	{
		stack[++top] = item;
	}
}

char Pop()//스택에서 데이터를 팝(삭제)하는 함수
{
	if (isEmpty())
	{
		printf("\n스택없음");
	}
	else
	{
		return stack[top--];
	}
}

void printStack()//스택에 현재 원소를 출력하는 연산
{
	printf("\nSTACK[");
	for (int i = 0; i <= top; i++)
	{
		printf(" %c ", stack[i]);
	}
	printf("]\n");
}

//스택적용 메인함수
int  main()
{
	char item[50];//스택에 삽입하는 원소
	char str;
	printf("\n5자리 문자열을 입력하세요 : "); 
	gets_s(item);//입력받기

	//입력받은 문자열을 스택에 삽입
	for (int i = 0; i < 5; i++)
	{
		Push(item[i]);
	}
	printStack(); //한번 출력

	printf("\n역순 문자열: "); //뽑아내면서 출력
	for (int i = 0; i < 5; i++)
	{
		str =  Pop(); //마지막 들어온 문자부터 역순으로 출력
		printf("%c",str);
	}
	
	return 0;
}
