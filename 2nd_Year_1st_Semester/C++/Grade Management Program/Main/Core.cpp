
#include "MainManager.h"

int main()
{
	CMainManager* mainprocess = new CMainManager();//�޸� �Ҵ�
	mainprocess->Initialize();//�ʱ�ȭ
	mainprocess->Run();//����
	delete mainprocess; //�޸� ����
	return 0;
}