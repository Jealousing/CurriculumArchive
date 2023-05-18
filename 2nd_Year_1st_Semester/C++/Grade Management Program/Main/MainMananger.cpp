#include "MainManager.h"

CMainManager::CMainManager()  //������ + �ּҰ� �ʱ�ȭ
{
	mDesignMng = nullptr;
	mGraphicMng = nullptr;
	mProgramMng = nullptr;
}

void CMainManager::Initialize() //�޸��Ҵ�
{
	mDesignMng = new CDesignManager();
	mGraphicMng = new CGraphicManager();
	mProgramMng = new CProgrammingManager();
}


CMainManager::~CMainManager() //�Ҹ��� �ּҰ��������� ����Ʈ!
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

int CMainManager::ShowMenu() //�޴��� ������
{
	int select;
	
	cout << "<�������α׷�>" << endl;
	cout << "1.�����Է�" << endl;
	cout << "2.�������" << endl;
	cout << "3.������ȸ" << endl;
	cout << "4.����" << endl;
	cout << "����:";
	cin >> select;

	return select;

}

void CMainManager::Run()
{
	int select;	
	
	while (1)
	{
		select = ShowMenu(); //�޴�����
		
		if (select == EXIT) //����
		{
			break;
		}

		CManager* manager = MajorSelect(); //�а� ����
		switch (select) //�޴� ���ÿ����� ó��
		{
		case INPUT:			
			manager->Input(); //�����Է�
			break;
		case SHOWDATA:
			manager->Display(); //�������
			break;
		case SEARCHDATA:
			manager->Search(); //������ȸ
			break;		
		default:
			cout << "�߸� �Է��߽��ϴ� �ٽ� �Է��ϼ��� " << endl;
		}
	}
}

CManager* CMainManager::MajorSelect() //�а�����
{
	while (1)
	{
		cout << "(���α׷���:1, �׷���:2, ��ȹ:3)" << endl;
		cout << "����:";
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
			cout << "�߸� �Է��߽��ϴ�." << endl;
			continue;
		}

		break;
	}

	return nullptr;

}

