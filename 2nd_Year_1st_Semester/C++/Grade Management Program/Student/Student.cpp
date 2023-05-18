#include "Student.h"

CStudent::CStudent(const char* _name, const char* _id) //생성자
{//받아온것을 저장
	strcpy(mId, _id); //학번
	strcpy(mName, _name); //이름

	memset(mSubject, 0, sizeof(mSubject)); //mSubject크기만큼 메모리 세팅
	mGrade = 0.0;//평균학점 초기화
}

CStudent::~CStudent()//소멸자
{

}

void CStudent::SetId(const char* _id) //학번 저장
{
	strcpy(mId, _id); 
}

void CStudent::SetName(const char* _name) //이름 저장
{
	strcpy(mName, _name);
}

const char* CStudent::GetId() //학번불러오기
{
	return mId;
}

const char* CStudent::GetName() //이름불러오기
{
	return mName;

}

void CStudent::SetSubject(int _sub, int _index) //과목 점수 입력
{
	mSubject[_index] = _sub;

}

int CStudent::GetSubject(int _index) //과목 점수 불러오기
{
	return mSubject[_index];
}

double CStudent::GetGrade() //학점 불러오기
{
	return mGrade;
}

void CStudent::CalculateGrade() //학점 계산
{
	double temp = 0.0; //합

	for (int i = 0; i < 3; i++)
	{
		int score = mSubject[i] / 10; //몫
		int m = mSubject[i] % 10; //나머지

		switch (score)
		{
		case 10:
		case 9:
			if (m>4)
			{
				temp = temp + (4.5 * 3); //A++
			}
			else
			{
				temp = temp + (4.0 * 3);//A
			}
			break;
		case 8:
			if (m > 4)
			{
				temp = temp + (3.5 * 3); //B+
			}
			else
			{
				temp = temp + (3.0 * 3);//B
			}

			break;
		case 7:
			if (m > 4)
			{
				temp = temp + (2.5 * 3);//C+
			}
			else
			{
				temp = temp + (2.0 * 3);//C
			}
			break;
		case 6:
			if (m > 4)
			{
				temp = temp + (1.5 * 3);//D+
			}
			else
			{
				temp = temp + (1.0 * 3);//D
			}
			//F?
			break;
		}
	}

	mGrade = temp / 9.0; //학점 평균
}
