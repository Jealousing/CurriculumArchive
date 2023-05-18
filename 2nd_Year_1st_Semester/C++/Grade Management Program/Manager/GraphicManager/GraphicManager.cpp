#include "GraphicManager.h"
const char* CGraphicManager::mSubjectName[] = { "2D", "3D", "�����" };//��������

CGraphicManager::CGraphicManager()//������
{
	

}

CGraphicManager::~CGraphicManager() //�Ҹ���
{

	
}

void CGraphicManager::Input()//�Է�
{
	char name[NAMESIZE]; //�̸�
	char id[IDSIZE]; //�й�
	int  score;// ����

	cout << "�̸�:";//�̸��Է¹ޱ�
	cin >> name;

	sprintf(id, "20%s%03d", GRAPHIC_CODE, mStuCount + 1);// �й��� ����

	CStudent* ptr = new CStudent(name, id); //�޸� �Ҵ�

	for (int i = 0; i < sizeof(mSubjectName) / sizeof(char*); i++) //������ �Է¹���
	{
		cout << mSubjectName[i] << ":"; //�����̸� ���
		cin >> score; //������ ���� �Է�
		ptr->SetSubject(score, i); //����
	}

	mStudentList[mStuCount++] = ptr; //�����ϰ� �л����� �ø��鼭 ������
}

void CGraphicManager::Display()// ���
{

	for (int i = 0; i < mStuCount; i++)//�л��� ��ŭ �ݺ�
	{
		cout << "�̸�:" << mStudentList[i]->GetName() << endl; //�̸����
		cout << "�й�:" << mStudentList[i]->GetId() << endl; //�й����

		for (int j = 0; j < sizeof(mSubjectName) / sizeof(char*); j++) //������ ���� ���
		{
			cout << mSubjectName[j] << ":";
			cout << mStudentList[i]->GetSubject(j) << endl;
		}

		mStudentList[i]->CalculateGrade(); //���� ���

		cout << "����:";
		cout << mStudentList[i]->GetGrade() << endl; //��� ����

	}
}

void CGraphicManager::Search()//�л��˻�
{
	char id[IDSIZE];
	cout << "�й�:";
	cin >> id; //�й��˻�

	for (int i = 0; i < mStuCount; i++)//�л�����ŭ �ݺ�
	{
		if (!strcmp(mStudentList[i]->GetId(), id)) //���ڿ� �� �й��� ��ġ�ϸ� ����
		{
			cout << "�̸�:" << mStudentList[i]->GetName() << endl;//�̸����
			cout << "�й�:" << mStudentList[i]->GetId() << endl;//�й����

			for (int j = 0; j < sizeof(mSubjectName) / sizeof(char*); j++)
			{
				cout << mSubjectName[j] << ":";
				cout << mStudentList[i]->GetSubject(j) << endl; //���� �������
			}

			cout << "����:";
			cout << mStudentList[i]->GetGrade() << endl;//��� ���� ���
			break;
		}
	}
}
