 #include "Manager.h"

CManager::CManager() //������ 
{
	memset(mStudentList, 0, sizeof(mStudentList)); //mStudentListũ�⸸ŭ �޸��ʱ�ȭ
	mStuCount = 0;//�л��� �ʱ�ȭ
}
CManager::~CManager()//�Ҹ���
{
	for (int i = 0; i < mStuCount; i++)//�л�����ŭ �޸� ����
	{
		delete mStudentList[i];
	}
}