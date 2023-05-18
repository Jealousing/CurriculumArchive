#include <stdio.h>

int i; //������� �˻��Ǵ��� �˰����ִ� ����
void func(const int E[], int begin, int end, int key)//���� �˻� �Լ�
{
	int middle;
	i++;//�˻� Ƚ�� ����
	if (begin == end)
	{
		if (key == E[begin])printf("%d��°�� �˻� ����!\n\n", i);
		else printf("%d��°�� �˻� ����!\n\n", i);
		return;
	}

	middle = (begin + end) / 2; //�߰���ġ

	if (key == E[middle]) //�߰���ġ�� ������ �ٷ� ����
	{
		printf("%d ��°�� �˻�����!!!\n", i);
	}
	else if (key < E[middle] && middle - 1 >= begin)//�߰����� �տ��������
	{
		func(E, begin, middle - 1, key);
	}
	else if (key > E[middle] && middle + 1 <= end)//�߰����� �ڿ��������
	{
		func(E, middle + 1, end, key);
	}
	else//���������� ������ ����
	{
		printf("%d ��°�� �˻�����!!!\n", i);
	}
}


int main(void)
{
	int E[] = { 3,5,6,9,10,16,21,23,27,31,33,40,41,43,44,59 };//int�� �迭�� �־���.
	int n = 16;//�迭 ����

	i = 0;
	printf("\n%d�� �˻��մϴ�: ", 21);
	func(E, 0, n - 1, 21);

	i = 0;
	printf("\n%d�� �˻��մϴ�: ", 60);
	func(E, 0, n - 1, 60);

	i = 0;
	printf("\n%d�� �˻��մϴ�: ", 41);
	func(E, 0, n - 1, 41);


	return 0;
}
