#ifndef CMAIN_H
#define CMAIN_H
#include "Global.h"
#include "../Manager/DesignManager/DesignManager.h"
#include "../Manager/GraphicManager/GraphicManager.h"
#include "../Manager/ProgramManager/ProgrammingManager.h"

class CMainManager
{
private://학과
	CDesignManager*				mDesignMng;		//기획
	CGraphicManager*			mGraphicMng;	//그래픽
	CProgrammingManager*		mProgramMng;	//프로그래밍
	
public:
	CMainManager();	//생성자
	~CMainManager(); //소멸자

	void			Initialize();	//초기화
	int				ShowMenu();		//메뉴
	CManager*		MajorSelect(); //학과선택

	void			Run(); //진행

};
#endif