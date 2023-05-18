#ifndef CMANAGER_H
#define CMANAGER_H
#include "../Main/Global.h"
#include "../Student/Student.h"

class CManager
{
protected:
	CStudent*					mStudentList[MAXSTUDENT];//학생정보
	int							mStuCount;//학생수

public:
	CManager();//생성자
	//가상함수로 파생클래스에서 재정의
	virtual ~CManager();	 //소멸자
	virtual void Input() = 0; //입력
	virtual void Display()=0;//출력
	virtual void Search()=0;//검색
};
#endif