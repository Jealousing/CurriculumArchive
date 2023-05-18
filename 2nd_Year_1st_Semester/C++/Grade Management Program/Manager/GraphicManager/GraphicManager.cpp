#include "GraphicManager.h"
const char* CGraphicManager::mSubjectName[] = { "2D", "3D", "드로잉" };//수업제목

CGraphicManager::CGraphicManager()//생성자
{
	

}

CGraphicManager::~CGraphicManager() //소멸자
{

	
}

void CGraphicManager::Input()//입력
{
	char name[NAMESIZE]; //이름
	char id[IDSIZE]; //학번
	int  score;// 점수

	cout << "이름:";//이름입력받기
	cin >> name;

	sprintf(id, "20%s%03d", GRAPHIC_CODE, mStuCount + 1);// 학번을 지정

	CStudent* ptr = new CStudent(name, id); //메모리 할당

	for (int i = 0; i < sizeof(mSubjectName) / sizeof(char*); i++) //성적을 입력받음
	{
		cout << mSubjectName[i] << ":"; //수업이름 출력
		cin >> score; //수업별 성적 입력
		ptr->SetSubject(score, i); //저장
	}

	mStudentList[mStuCount++] = ptr; //저장하고 학생수를 늘리면서 끝내기
}

void CGraphicManager::Display()// 출력
{

	for (int i = 0; i < mStuCount; i++)//학생수 만큼 반복
	{
		cout << "이름:" << mStudentList[i]->GetName() << endl; //이름출력
		cout << "학번:" << mStudentList[i]->GetId() << endl; //학번출력

		for (int j = 0; j < sizeof(mSubjectName) / sizeof(char*); j++) //수업별 성적 출력
		{
			cout << mSubjectName[j] << ":";
			cout << mStudentList[i]->GetSubject(j) << endl;
		}

		mStudentList[i]->CalculateGrade(); //학점 계산

		cout << "평점:";
		cout << mStudentList[i]->GetGrade() << endl; //평균 학점

	}
}

void CGraphicManager::Search()//학생검색
{
	char id[IDSIZE];
	cout << "학번:";
	cin >> id; //학번검색

	for (int i = 0; i < mStuCount; i++)//학생수만큼 반복
	{
		if (!strcmp(mStudentList[i]->GetId(), id)) //문자열 비교 학번이 일치하면 진행
		{
			cout << "이름:" << mStudentList[i]->GetName() << endl;//이름출력
			cout << "학번:" << mStudentList[i]->GetId() << endl;//학번출력

			for (int j = 0; j < sizeof(mSubjectName) / sizeof(char*); j++)
			{
				cout << mSubjectName[j] << ":";
				cout << mStudentList[i]->GetSubject(j) << endl; //과목별 점수출력
			}

			cout << "평점:";
			cout << mStudentList[i]->GetGrade() << endl;//평균 학점 출력
			break;
		}
	}
}
