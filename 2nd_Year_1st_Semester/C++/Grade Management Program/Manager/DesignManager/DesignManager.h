#ifndef CDESIGN_MANAGER_H
#define CDESIGN_MANAGER_H
#include "../../Student/Student.h"
#include "../Manager.h"

#define DESIGN_CODE "031" //�а���ȣ

class CDesignManager: public CManager //Ŭ���� ���
{
private:
	static const char*			mSubjectName[];	

public:
	CDesignManager(); //������
	~CDesignManager(); //�Ҹ���

	virtual void Input(); //�Է�
	virtual void Display();	//���
	virtual void Search();	//�������
};
#endif