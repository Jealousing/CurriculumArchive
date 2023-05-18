#include "Student.h"

CStudent::CStudent(const char* _name, const char* _id) //������
{//�޾ƿ°��� ����
	strcpy(mId, _id); //�й�
	strcpy(mName, _name); //�̸�

	memset(mSubject, 0, sizeof(mSubject)); //mSubjectũ�⸸ŭ �޸� ����
	mGrade = 0.0;//������� �ʱ�ȭ
}

CStudent::~CStudent()//�Ҹ���
{

}

void CStudent::SetId(const char* _id) //�й� ����
{
	strcpy(mId, _id); 
}

void CStudent::SetName(const char* _name) //�̸� ����
{
	strcpy(mName, _name);
}

const char* CStudent::GetId() //�й��ҷ�����
{
	return mId;
}

const char* CStudent::GetName() //�̸��ҷ�����
{
	return mName;

}

void CStudent::SetSubject(int _sub, int _index) //���� ���� �Է�
{
	mSubject[_index] = _sub;

}

int CStudent::GetSubject(int _index) //���� ���� �ҷ�����
{
	return mSubject[_index];
}

double CStudent::GetGrade() //���� �ҷ�����
{
	return mGrade;
}

void CStudent::CalculateGrade() //���� ���
{
	double temp = 0.0; //��

	for (int i = 0; i < 3; i++)
	{
		int score = mSubject[i] / 10; //��
		int m = mSubject[i] % 10; //������

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

	mGrade = temp / 9.0; //���� ���
}
