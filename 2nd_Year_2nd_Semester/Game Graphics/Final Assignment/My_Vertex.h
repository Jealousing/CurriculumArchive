#pragma once
#include "MY_Monster.h"
#include "My_Player.h"

#define MAX_MONSTER 100

class Vertex
{
private:
	int player_color;
	int monster_color;
	
	int monster_numset = 0;

	int death_num = 0;
	bool Iscrash = false;
public:
	Player _player[2];
	Monster _Monster[MAX_MONSTER];

	void Make_Player(Matrix1x4* _VertexData);
	void Make_Monster(int Monster_num, RECT _clientRect, int _count);

	void Draw_Player(int _select, HDC _hdc, float* _data_space1, float* _data_space2);
	void Draw_Monster(HDC _hdc, float* _data_space1, float* _data_space2);

	void Management_Monst();
};

void Vertex::Make_Player(Matrix1x4* _VertexData)
{
	for (int i = 0; i < 2; i++)
	{
		_player[i].SetPlayer(_VertexData);
	}
}

void Vertex::Make_Monster(int Monster_num, RECT _clientRect,  int _count)
{
	monster_numset = Monster_num;
	for (int i = 0; i < Monster_num; i++)
	{
		_Monster[i].MakeMonster(_clientRect, _count);
	}
}


void Vertex::Draw_Player(int _select, HDC _hdc, float* _data_space1, float* _data_space2)
{
	_player[_select].DrowTriangle(_hdc, _data_space1, _data_space2, Iscrash);
}

void Vertex::Draw_Monster(HDC _hdc, float* _data_space1, float* _data_space2)
{
	//���� �׸���
	for (int i = 0; i < monster_numset; i++)
	{
		_Monster[i].DrowTriangle(_hdc, _data_space1, _data_space2);
	}
}

void Vertex::Management_Monst()
{
	//���� ���� ���� ������ ������ ���ڰ� ������ ����
	if (death_num == monster_numset)
	{
		Iscrash = false;
		return;
	}


	int CrashCheck = 0;
	//������ ����
	for (int i = 0; i < monster_numset; i++)
	{
		if (_Monster[i].Isdeath) //�׾����� ���� x
			continue;

		//�÷��̾��� Ÿ������ ���ƴٴ�
		_Monster[i].Roaming(_player[0]);

		//�÷��̾�� �浹�ߴ��� üũ
		if (_Monster[i].DeathCheck())
		{
			//�浹Ƚ�� üũ
			CrashCheck++;
			//�浹�ϸ� �� ���� ����ó��
			_Monster[i].Isdeath = true;
			//�÷��̾ �浹�ߴٴ� ���� �˷���
			Iscrash = true;
			//�������� ������ ����
			death_num++;
		}

		if (CrashCheck == 0)
		{
			Iscrash = false;
		}
	}
}