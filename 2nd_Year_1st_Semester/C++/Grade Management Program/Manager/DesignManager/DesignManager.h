#ifndef CDESIGN_MANAGER_H
#define CDESIGN_MANAGER_H
#include "../../Student/Student.h"
#include "../Manager.h"

#define DESIGN_CODE "031" //학과번호

class CDesignManager: public CManager //클래스 상속
{
private:
	static const char*			mSubjectName[];	

public:
	CDesignManager(); //생성자
	~CDesignManager(); //소멸자

	virtual void Input(); //입력
	virtual void Display();	//출력
	virtual void Search();	//선택출력
};
#endif