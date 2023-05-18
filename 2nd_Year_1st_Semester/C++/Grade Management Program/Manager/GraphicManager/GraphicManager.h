#ifndef CGRAPHIC_MANAGER_H
#define CGRAPHIC_MANAGER_H
#include "../../Student/Student.h"
#include "../Manager.h"


#define GRAPHIC_CODE "033"//학과번호


class CGraphicManager : public CManager  //클래스 상속
{
private:
	static const char*			mSubjectName[];

public:
	CGraphicManager(); //생성자
	~CGraphicManager();//소멸자

	virtual void Input();//입력
	virtual void Display();//출력
	virtual void Search();//선택출력

};
#endif