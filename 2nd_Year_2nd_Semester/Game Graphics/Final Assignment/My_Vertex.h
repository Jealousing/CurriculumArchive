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
	//몬스터 그리기
	for (int i = 0; i < monster_numset; i++)
	{
		_Monster[i].DrowTriangle(_hdc, _data_space1, _data_space2);
	}
}

void Vertex::Management_Monst()
{
	//죽은 몬스터 수와 생성된 몬스터의 숫자가 같으면 실행
	if (death_num == monster_numset)
	{
		Iscrash = false;
		return;
	}


	int CrashCheck = 0;
	//움직임 관리
	for (int i = 0; i < monster_numset; i++)
	{
		if (_Monster[i].Isdeath) //죽었으면 실행 x
			continue;

		//플레이어을 타겟으로 돌아다님
		_Monster[i].Roaming(_player[0]);

		//플레이어와 충돌했는지 체크
		if (_Monster[i].DeathCheck())
		{
			//충돌횟수 체크
			CrashCheck++;
			//충돌하면 그 몬스터 죽음처리
			_Monster[i].Isdeath = true;
			//플레이어도 충돌했다는 것을 알려줌
			Iscrash = true;
			//죽은몬스터 마릿수 증가
			death_num++;
		}

		if (CrashCheck == 0)
		{
			Iscrash = false;
		}
	}
}