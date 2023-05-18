#ifndef CPROGRAMMING_MANAGER_H
#define CPROGRAMMING_MANAGER_H
#include "../../Student/Student.h"
#include "../Manager.h"

#define PROGRAM_CODE "032"//학과번호

class CProgrammingManager :public CManager //클래스 상속
{
private:
	static const char*			mSubjectName[];	

public:
	CProgrammingManager();//생성자
	~CProgrammingManager();//소멸자

	virtual void Input(); //입력
	virtual void Display(); //출력
	virtual void Search();//선택출력
	
};
#endif