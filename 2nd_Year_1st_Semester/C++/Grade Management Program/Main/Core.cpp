
#include "MainManager.h"

int main()
{
	CMainManager* mainprocess = new CMainManager();//메모리 할당
	mainprocess->Initialize();//초기화
	mainprocess->Run();//시작
	delete mainprocess; //메모리 삭제
	return 0;
}