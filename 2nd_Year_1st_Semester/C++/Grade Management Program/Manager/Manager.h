#ifndef CMANAGER_H
#define CMANAGER_H
#include "../Main/Global.h"
#include "../Student/Student.h"

class CManager
{
protected:
	CStudent*					mStudentList[MAXSTUDENT];//�л�����
	int							mStuCount;//�л���

public:
	CManager();//������
	//�����Լ��� �Ļ�Ŭ�������� ������
	virtual ~CManager();	 //�Ҹ���
	virtual void Input() = 0; //�Է�
	virtual void Display()=0;//���
	virtual void Search()=0;//�˻�
};
#endif