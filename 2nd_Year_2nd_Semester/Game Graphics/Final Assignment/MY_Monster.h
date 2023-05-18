#pragma once
#include <list>
#include <cstdlib>
#include <ctime>
#include <random>
#include "My_Player.h"



#define MAXPOINT 128
#define ANGLESIZE 5
#define RADIUS 5
#define VERTEXNUM 3
#define MOVENUM 20
#define RANDDISTANC 500

#define AGGRODISTANC 300
#define DEATHDISTANC 75

class Monster
{
private:

	My_Vector Centroid;
	int Pointnum;				//몇개의 이동 웨이포인트가 있는지
	int Roam;						//어디이동했는지
	bool IsRoaming = false;
	bool IsTarget = false;
	int Movenum;
	float Radius = 0;

	My_Vector WayPointPos[MAXPOINT];
	My_Vector MovePos;
	My_Vector SaveDisTanc;

	Matrix1x4 Vertex[3];
	
	Player _playerdata;

public:

	bool Isdeath = false;

	Monster();
	void MakeMonster(RECT _clientRect,int _count);
	void GetVertex(Matrix1x4* _ver);
	void MakeVertex();
	void WayPoint(RECT _clientRect);
	void Roaming(Player _player);

	void EquationLine(float* PosX, float* PosY, Matrix1x4 data1, Matrix1x4 data2);
	void DrowLine(HDC _hdc, float* PosX, float* PosY, Matrix1x4 data1, Matrix1x4 data2);
	void DrowTriangleCenter(HDC _hdc);
	void DrowTriangle(HDC _hdc, float* PosX, float* PosY);

	My_Vector GetTriangleCenter(Matrix1x4* data);
	void Move(float MoveX, float MoveY, float MoveZ);
	void DistancePos(float _MoveX, float _MoveY);

	float LengthPts(float x1, float y1, float x2, float y2);
	BOOL InCircle();
	BOOL DeathCheck();
	void RadiusCheck();
};

Monster::Monster()
{
	Centroid.Set_X(0);
	Centroid.Set_Y(0);
	MakeVertex();
}

void Monster::MakeMonster(RECT _clientRect,int _count)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> randX(0, _clientRect.right-100);
	uniform_real_distribution<float> randY(0, _clientRect.bottom-100);
	srand(time(NULL));
	Centroid.Set_X(randX(gen));
	Centroid.Set_Y(randY(gen));
	Pointnum = _count;
	Roam = 0;
	Movenum = 0;
	MakeVertex();
	WayPoint(_clientRect);
}

void Monster::GetVertex(Matrix1x4 * _ver)
{
	_ver[0] = Vertex[0];
	_ver[1] = Vertex[1];
	_ver[2] = Vertex[2];
}

void Monster::MakeVertex()
{
	Vertex[0].m[0][0]	= Centroid.Get_X();				Vertex[0].m[0][1]		= Centroid.Get_Y() - 34.6410f;		
	Vertex[0].m[0][2]	= 1.0f;									Vertex[0].m[0][3] = 1.0f;

	Vertex[1].m[0][0]		= Centroid.Get_X()- 30.0f;		Vertex[1].m[0][1]		= Centroid.Get_Y()+17.3205f;				
	Vertex[1].m[0][2]		= 1.0f;									Vertex[1].m[0][3] = 1.0f;

	Vertex[2].m[0][0]	= Centroid.Get_X()+ 30.0f;		Vertex[2].m[0][1]		= Centroid.Get_Y()+17.3205f;				
	Vertex[2].m[0][2]		= 1.0f;									Vertex[2].m[0][3] = 1.0f;
}

void Monster::WayPoint(RECT _clientRect)
{
	random_device rd;
	mt19937 gen(rd());
	float max_x = Centroid.Get_X();
	float max_y = Centroid.Get_Y();
	float min_x = Centroid.Get_X();
	float min_y = Centroid.Get_Y();
	if (Centroid.Get_X() + RANDDISTANC < _clientRect.right)
	{
		max_x += RANDDISTANC;
	}
	if (Centroid.Get_Y() + RANDDISTANC < _clientRect.bottom)
	{
		max_y += RANDDISTANC;
	}
	if (Centroid.Get_X() - RANDDISTANC > _clientRect.left)
	{
		min_x -= RANDDISTANC;
	}
	if (Centroid.Get_Y() - RANDDISTANC > _clientRect.top)
	{
		min_y -= RANDDISTANC;
	}

	
	uniform_real_distribution<float> randX(min_x, max_x);
	uniform_real_distribution<float> randY(min_y, max_y);
	for (int i = 0; i < Pointnum; i++)
	{
		float posX = randX(gen);
		float posY = randY(gen);
		WayPointPos[i].SetPos(posX, posY, 0);
	}
}
void Monster::Roaming(Player _player)
{
	_playerdata = _player;

	if (InCircle())
	{
		if (IsTarget)
		{
			if (Movenum == MOVENUM)
			{
				Movenum = 0;
				IsTarget = false;
				return;
			}
			DistancePos(_playerdata.GetCentroidPos().Get_X(), _playerdata.GetCentroidPos().Get_Y());

			MovePos.Set_X(SaveDisTanc.Get_X() / MOVENUM);
			MovePos.Set_Y(SaveDisTanc.Get_Y() / MOVENUM);

			Move(MovePos.Get_X(), MovePos.Get_Y(), 0);
			Movenum++;
			return;
		}
		else
		{
			Movenum = 0;
			IsTarget = true;
			return;
		}
		return;
	}

	if (IsRoaming)
	{
		if (Movenum == MOVENUM)
		{
			Movenum = 0;
			IsRoaming = false;
			return;
		}
		Move(MovePos.Get_X(), MovePos.Get_Y(),0);
		Movenum++;
	}
	else
	{
		if (Roam == Pointnum-1)
		{
			Roam = 0;
		}
		DistancePos(WayPointPos[Roam].Get_X(),WayPointPos[Roam++].Get_Y());
		MovePos.Set_X(SaveDisTanc.Get_X() / MOVENUM);
		MovePos.Set_Y(SaveDisTanc.Get_Y() / MOVENUM);
		IsRoaming = true;

	}

}

//직선의 방정식
void Monster::EquationLine(float* PosX, float* PosY, Matrix1x4 data1, Matrix1x4 data2)
{
	int a = data1.m[0][0] - data2.m[0][0];
	int b = data1.m[0][1] - data2.m[0][1];

	bool select = FALSE;

	if (abs(a) > abs(b))
	{
		select = FALSE;
	}
	else if (abs(b) > abs(a))
	{
		select = TRUE;
	}

	for (int i = 0; i < abs(a); i++)
	{
		if (data1.m[0][0] > data2.m[0][0])
		{
			PosX[i] = data1.m[0][0] - i;
		}
		else if (data2.m[0][0] > data1.m[0][0])
		{
			PosX[i] = data1.m[0][0] + i;
		}
	}
	for (int i = 0; i < abs(b); i++)
	{
		if (data1.m[0][1] > data2.m[0][1])
		{
			PosY[i] = data1.m[0][1] - i;
		}
		else if (data2.m[0][1] > data1.m[0][1])
		{
			PosY[i] = data1.m[0][1] + i;
		}
	}

	if (data1.m[0][0] == data2.m[0][0])
	{
		data1.m[0][0] - 0.00001;
	}
	if (data1.m[0][1] == data2.m[0][1])
	{
		data1.m[0][1] - 0.00001;
	}

	if (select == FALSE)
	{
		for (int i = 0; i < 1024; i++)
		{
			PosY[i] = (((data2.m[0][1] - data1.m[0][1]) / (data2.m[0][0] - data1.m[0][0])) * (PosX[i] - data1.m[0][0]) + data1.m[0][1]);
		}
	}
	if (select == TRUE)
	{
		for (int i = 0; i < 1024; i++)
		{
			PosX[i] = (((data2.m[0][0] - data1.m[0][0]) / (data2.m[0][1] - data1.m[0][1])) * (PosY[i] - data1.m[0][1]) + data1.m[0][0]);
		}
	}
}
//두점을 이어 선으로 그리는 함수
void Monster::DrowLine(HDC _hdc, float* PosX, float* PosY, Matrix1x4 data1, Matrix1x4 data2)
{
	int a = data1.m[0][0] - data2.m[0][0];
	int b = data1.m[0][1] - data2.m[0][1];


	if (abs(a) > abs(b))
	{
		EquationLine(PosX, PosY, data1, data2);
		for (int i = 0; i < abs(a); i++)//출력
		{
			if (Isdeath)
			{
				SetPixel(_hdc, PosX[i], PosY[i], RGB(255, 255, 255));
			}
			else
			{
				SetPixel(_hdc, PosX[i], PosY[i], RGB(255, 0, 0));
			}
		
		}
	}
	else if (abs(b) > abs(a))
	{
		EquationLine(PosX, PosY, data1, data2);
		for (int i = 0; i < abs(b); i++)//출력
		{
			if (Isdeath)
			{
				SetPixel(_hdc, PosX[i], PosY[i], RGB(255, 255, 255));
			}
			else
			{
				SetPixel(_hdc, PosX[i], PosY[i], RGB(255, 0, 0));
			}
		}
	}
}
//삼각형의 무게중심을 원으로 그리는 함수
void Monster::DrowTriangleCenter(HDC _hdc)
{
	HBRUSH Brush, oBrush;
	Brush = CreateSolidBrush(RGB(0, 0, 255));
	oBrush = (HBRUSH)SelectObject(_hdc, Brush);
	Ellipse(_hdc, Centroid.Get_X() - RADIUS, Centroid.Get_Y() - RADIUS, Centroid.Get_X() + RADIUS, Centroid.Get_Y() + RADIUS);
	SelectObject(_hdc, oBrush);
	DeleteObject(Brush);
}
//삼각형 그리는함수
void Monster::DrowTriangle(HDC _hdc, float* PosX, float* PosY)
{
	//반지름으로 테스트
	//Ellipse(_hdc, Centroid.Get_X() - Radius, Centroid.Get_Y() - Radius, Centroid.Get_X() + Radius, Centroid.Get_Y() + Radius);
	MakeVertex();
	DrowLine(_hdc, PosX, PosY, Vertex[0], Vertex[1]);
	DrowLine(_hdc, PosX, PosY, Vertex[0], Vertex[2]);
	DrowLine(_hdc, PosX, PosY, Vertex[1], Vertex[2]);
	DrowTriangleCenter(_hdc);
}

My_Vector Monster::GetTriangleCenter(Matrix1x4* data)
{
	My_Vector CenterT;
	CenterT.Set_X(0);
	CenterT.Set_Y(0);
	for (int i = 0; i < VERTEXNUM; i++)
	{
		CenterT.Set_X(CenterT.Get_X() + data[i].m[0][0]);
		CenterT.Set_Y(CenterT.Get_Y() + data[i].m[0][1]);
	}
	CenterT.Set_X(CenterT.Get_X() / VERTEXNUM);
	CenterT.Set_Y(CenterT.Get_Y() / VERTEXNUM);
	return CenterT;
}


void Monster::Move(float MoveX, float MoveY, float MoveZ)
{
	Matrix4x4 temp = MatrixTranslate(MoveX, MoveY, MoveZ);
	for (int i = 0; i < VERTEXNUM; i++)
	{
		Vertex[i] = Multiple(Vertex[i], temp);
	}
	Centroid = GetTriangleCenter(Vertex);
}

void Monster::DistancePos(float _MoveX, float _MoveY)
{
	SaveDisTanc.Set_X(_MoveX - Centroid.Get_X());
	SaveDisTanc.Set_Y(_MoveY - Centroid.Get_Y());
}


float Monster::LengthPts(float x1, float y1, float x2, float y2)
{
	return(sqrt((float)((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)))); 
}

BOOL Monster::InCircle()
{
	if (LengthPts(Centroid.Get_X(), Centroid.Get_Y(), _playerdata.GetCentroidPos().Get_X(), _playerdata.GetCentroidPos().Get_Y()) < AGGRODISTANC)
		return TRUE;
	else
		return FALSE;
}

//충돌 부분입니다
BOOL Monster::DeathCheck()
{
	RadiusCheck();
	if (LengthPts(Centroid.Get_X(), Centroid.Get_Y(), _playerdata.GetCentroidPos().Get_X(), _playerdata.GetCentroidPos().Get_Y())  < Radius + _playerdata.GetRadius())
		return TRUE;

		return FALSE;
}

void Monster::RadiusCheck()
{
	for (int i = 0; i < 3; i++)
	{
		float temp = LengthPts(Centroid.Get_X(), Centroid.Get_Y(), Vertex[i].m[0][0], Vertex[i].m[0][1]);

		if (temp > Radius)
		{
			Radius = temp;
		}
	}
}