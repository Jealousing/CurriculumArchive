#ifndef CMAIN_H
#define CMAIN_H
#include "Global.h"
#include "../Manager/DesignManager/DesignManager.h"
#include "../Manager/GraphicManager/GraphicManager.h"
#include "../Manager/ProgramManager/ProgrammingManager.h"

class CMainManager
{
private://�а�
	CDesignManager*				mDesignMng;		//��ȹ
	CGraphicManager*			mGraphicMng;	//�׷���
	CProgrammingManager*		mProgramMng;	//���α׷���
	
public:
	CMainManager();	//������
	~CMainManager(); //�Ҹ���

	void			Initialize();	//�ʱ�ȭ
	int				ShowMenu();		//�޴�
	CManager*		MajorSelect(); //�а�����

	void			Run(); //����

};
#endif