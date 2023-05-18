 #include "Manager.h"

CManager::CManager() //생성자 
{
	memset(mStudentList, 0, sizeof(mStudentList)); //mStudentList크기만큼 메모리초기화
	mStuCount = 0;//학생수 초기화
}
CManager::~CManager()//소멸자
{
	for (int i = 0; i < mStuCount; i++)//학생수만큼 메모리 삭제
	{
		delete mStudentList[i];
	}
}