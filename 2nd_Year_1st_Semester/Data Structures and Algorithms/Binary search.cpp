#include <stdio.h>

int i; //몇번만에 검색되는지 알게해주는 변수
void func(const int E[], int begin, int end, int key)//이진 검색 함수
{
	int middle;
	i++;//검색 횟수 증가
	if (begin == end)
	{
		if (key == E[begin])printf("%d번째에 검색 성공!\n\n", i);
		else printf("%d번째에 검색 실패!\n\n", i);
		return;
	}

	middle = (begin + end) / 2; //중간위치

	if (key == E[middle]) //중간위치에 있으면 바로 성공
	{
		printf("%d 번째에 검색성공!!!\n", i);
	}
	else if (key < E[middle] && middle - 1 >= begin)//중간기준 앞에있을경우
	{
		func(E, begin, middle - 1, key);
	}
	else if (key > E[middle] && middle + 1 <= end)//중간기준 뒤에있을경우
	{
		func(E, middle + 1, end, key);
	}
	else//마지막까지 없으면 실패
	{
		printf("%d 번째에 검색실패!!!\n", i);
	}
}


int main(void)
{
	int E[] = { 3,5,6,9,10,16,21,23,27,31,33,40,41,43,44,59 };//int형 배열에 넣어줌.
	int n = 16;//배열 갯수

	i = 0;
	printf("\n%d를 검색합니다: ", 21);
	func(E, 0, n - 1, 21);

	i = 0;
	printf("\n%d를 검색합니다: ", 60);
	func(E, 0, n - 1, 60);

	i = 0;
	printf("\n%d를 검색합니다: ", 41);
	func(E, 0, n - 1, 41);


	return 0;
}
