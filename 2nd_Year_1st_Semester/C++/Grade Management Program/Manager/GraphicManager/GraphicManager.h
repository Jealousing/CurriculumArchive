#ifndef CGRAPHIC_MANAGER_H
#define CGRAPHIC_MANAGER_H
#include "../../Student/Student.h"
#include "../Manager.h"


#define GRAPHIC_CODE "033"//�а���ȣ


class CGraphicManager : public CManager  //Ŭ���� ���
{
private:
	static const char*			mSubjectName[];

public:
	CGraphicManager(); //������
	~CGraphicManager();//�Ҹ���

	virtual void Input();//�Է�
	virtual void Display();//���
	virtual void Search();//�������

};
#endif