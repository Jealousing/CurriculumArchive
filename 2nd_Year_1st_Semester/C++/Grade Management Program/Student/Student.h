#ifndef CSTUDENT_H
#define CSTUDENT_H
#include "../Main/Global.h"

class CStudent
{
private:
	char mId[IDSIZE];//학번
	char mName[NAMESIZE];//이름
	int mSubject[3];//과목별 점수
	double mGrade;//학점평균

public:
	CStudent(const char*, const char*);//생성자
	~CStudent();//소멸자
	//입력
	void SetId(const char*);//학번
	void SetName(const char*);//이름
	//가져오기
	const char* GetId();//학번
	const char* GetName();//이름

	void SetSubject(int, int); //성적입력
	int GetSubject(int);//성적출력

	double GetGrade();//학점출력

	void CalculateGrade();//학점계산
};
#endif