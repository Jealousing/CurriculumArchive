#ifndef CPROGRAMMING_MANAGER_H
#define CPROGRAMMING_MANAGER_H
#include "../../Student/Student.h"
#include "../Manager.h"

#define PROGRAM_CODE "032"//�а���ȣ

class CProgrammingManager :public CManager //Ŭ���� ���
{
private:
	static const char*			mSubjectName[];	

public:
	CProgrammingManager();//������
	~CProgrammingManager();//�Ҹ���

	virtual void Input(); //�Է�
	virtual void Display(); //���
	virtual void Search();//�������
	
};
#endif