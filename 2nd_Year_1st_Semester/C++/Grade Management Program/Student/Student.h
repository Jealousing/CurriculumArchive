#ifndef CSTUDENT_H
#define CSTUDENT_H
#include "../Main/Global.h"

class CStudent
{
private:
	char mId[IDSIZE];//�й�
	char mName[NAMESIZE];//�̸�
	int mSubject[3];//���� ����
	double mGrade;//�������

public:
	CStudent(const char*, const char*);//������
	~CStudent();//�Ҹ���
	//�Է�
	void SetId(const char*);//�й�
	void SetName(const char*);//�̸�
	//��������
	const char* GetId();//�й�
	const char* GetName();//�̸�

	void SetSubject(int, int); //�����Է�
	int GetSubject(int);//�������

	double GetGrade();//�������

	void CalculateGrade();//�������
};
#endif