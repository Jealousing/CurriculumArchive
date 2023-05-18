#include "MainManager.h"

CMainManager::CMainManager()  //생성자 + 주소값 초기화
{
	mDesignMng = nullptr;
	mGraphicMng = nullptr;
	mProgramMng = nullptr;
}

void CMainManager::Initialize() //메모리할당
{
	mDesignMng = new CDesignManager();
	mGraphicMng = new CGraphicManager();
	mProgramMng = new CProgrammingManager();
}


CMainManager::~CMainManager() //소멸자 주소값이있으면 딜리트!
{
	if (mDesignMng != nullptr)
	{
		delete mDesignMng;
	}

	if (mGraphicMng != nullptr)
	{
		delete mGraphicMng;
	}

	if (mProgramMng != nullptr)
	{
		delete mProgramMng;
	}

}

int CMainManager::ShowMenu() //메뉴를 보여줌
{
	int select;
	
	cout << "<성적프로그램>" << endl;
	cout << "1.성적입력" << endl;
	cout << "2.성적출력" << endl;
	cout << "3.성적조회" << endl;
	cout << "4.종료" << endl;
	cout << "선택:";
	cin >> select;

	return select;

}

void CMainManager::Run()
{
	int select;	
	
	while (1)
	{
		select = ShowMenu(); //메뉴선택
		
		if (select == EXIT) //종료
		{
			break;
		}

		CManager* manager = MajorSelect(); //학과 선택
		switch (select) //메뉴 선택에따른 처리
		{
		case INPUT:			
			manager->Input(); //성적입력
			break;
		case SHOWDATA:
			manager->Display(); //성적출력
			break;
		case SEARCHDATA:
			manager->Search(); //성적조회
			break;		
		default:
			cout << "잘못 입력했습니다 다시 입력하세요 " << endl;
		}
	}
}

CManager* CMainManager::MajorSelect() //학과선택
{
	while (1)
	{
		cout << "(프로그래밍:1, 그래픽:2, 기획:3)" << endl;
		cout << "선택:";
		int sel;
		cin >> sel;


		switch (sel)
		{
		case PROGRAMMING:
			return mProgramMng;
			break;
		case GRAPHIC:
			return mGraphicMng;
			break;
		case DESIGN:
			return mDesignMng;
			break;
		default:
			cout << "잘못 입력했습니다." << endl;
			continue;
		}

		break;
	}

	return nullptr;

}

