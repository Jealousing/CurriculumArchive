#pragma once
#include <iostream>
#include "My_Matrix.h"
#include "My_Vector.h"

#define VERTEXNUM 3
#define ANGLESIZE 5
#define RADIUS 5

class Player
{
private:
	My_Vector Centroid;
	My_Vector MovePos;
	Matrix1x4 Vertex[VERTEXNUM];
	float Radius = 999;
	int Color = 0;

public:
	Player()
	{
	}
	Player(Matrix1x4* _Vertex)
	{
		MovePos.Set_X(0);
		MovePos.Set_Y(0);
		for (int i = 0; i < VERTEXNUM; i++)
		{
			Vertex[i] = _Vertex[i];
		}
		Centroid= GetTriangleCenter(_Vertex);
	}
	void CopyPlayer(Player _copyPlayer);
	void SetPlayer(Matrix1x4* _Vertex);
	My_Vector GetMovePos();
	My_Vector GetCentroidPos();
	Matrix1x4 GetVertexPos(int _turn);
	void DistancePos(float _MoveX, float _MoveY);
	void Mresult(WPARAM _wParam);
	My_Vector GetTriangleCenter(Matrix1x4* data);
	void Move(float MoveX, float MoveY, float MoveZ);

	void EquationLine(float* PosX, float* PosY, Matrix1x4 data1, Matrix1x4 data2);
	void DrowLine(HDC _hdc, float* PosX, float* PosY, Matrix1x4 data1, Matrix1x4 data2, bool Iscrash);
	void DrowTriangleCenter(HDC _hdc);
	void DrowTriangle(HDC _hdc, float* PosX, float* PosY, bool Iscrash);

	float GetRadius();
};



void Player::CopyPlayer(Player _copyPlayer)
{
	Centroid = _copyPlayer.Centroid;
	MovePos = _copyPlayer.MovePos;
	for (int i = 0; i < VERTEXNUM; i++)
	{
		Vertex[i] = _copyPlayer.Vertex[i];
	}
}
void Player::SetPlayer(Matrix1x4* _Vertex)
{
	for (int i = 0; i < VERTEXNUM; i++)
	{
		Vertex[i] = _Vertex[i];
	}
	Centroid = GetTriangleCenter(_Vertex);
}
My_Vector Player::GetMovePos()
{
	return MovePos;
}
My_Vector Player::GetCentroidPos()
{
	return Centroid;
}
Matrix1x4 Player::GetVertexPos(int _turn)
{
	return Vertex[_turn];
}
void Player::DistancePos(float _MouseX, float _MouseY)
{
	MovePos.Set_X( _MouseX - Centroid.Get_X());
	MovePos.Set_Y( _MouseY - Centroid.Get_Y());
}
void Player::Mresult(WPARAM _wParam)
{
	Matrix4x4 returnMatrix;
	Matrix4x4 tempMatrix;

	DistancePos(0, 0);
	My_Vector saveCentroid = Centroid;
	returnMatrix = MatrixTranslate(MovePos.Get_X(), MovePos.Get_Y(), 0);

	//case ���� ��� 
	switch (_wParam)
	{
	case 'E':			//�ﰢ�� ũ������
		tempMatrix = MatrixScaling(1.1f, 1.1f, 0);
		break;
	case 'Q':			//�ﰢ�� ũ�Ⱘ��
		tempMatrix = MatrixScaling(0.9f, 0.9f, 1);
		break;
	case '1':			//x�� ȸ��
		tempMatrix = MatrixRotateX(ANGLESIZE);
		break;
	case '2':			//y�� ȸ��
		tempMatrix = MatrixRotateY(ANGLESIZE);
		break;
	case '3':			//z�� ȸ��
		tempMatrix = MatrixRotateZ(ANGLESIZE);
		break;
	case VK_F1:		//x�� ��ȸ��
		tempMatrix = MatrixRotateX(-ANGLESIZE);
		break;
	case VK_F2:		//y�� ��ȸ��
		tempMatrix = MatrixRotateY(-ANGLESIZE);
		break;
	case VK_F3:		//z�� ��ȸ��
		tempMatrix = MatrixRotateZ(-ANGLESIZE);
		break;
	}
	returnMatrix = Multiple(returnMatrix, tempMatrix);
	tempMatrix = MatrixTranslate(-MovePos.Get_X(), -MovePos.Get_Y() , 0);

	returnMatrix = Multiple(returnMatrix, tempMatrix);

	for (int i = 0; i < VERTEXNUM; i++)
	{
		Vertex[i] = Multiple(Vertex[i], returnMatrix);
	}
	Centroid = GetTriangleCenter(Vertex);
	
	//��ǥ����
	DistancePos(saveCentroid.Get_X(), saveCentroid.Get_Y());
	Move(MovePos.Get_X(), MovePos.Get_Y(), 0);

	
}
My_Vector Player::GetTriangleCenter(Matrix1x4* data)
{
	My_Vector CenterT;
	CenterT.Set_X(0);
	CenterT.Set_Y(0);
	for (int i = 0; i < VERTEXNUM; i++)
	{
		CenterT.Set_X(CenterT.Get_X() + data[i].m[0][0]);
		CenterT.Set_Y(CenterT.Get_Y() + data[i].m[0][1]);
	}
	CenterT.Set_X ( CenterT.Get_X() / VERTEXNUM);
	CenterT.Set_Y(  CenterT.Get_Y() / VERTEXNUM);
	return CenterT;
}
void Player::Move(float MoveX, float MoveY, float MoveZ)
{
	Matrix4x4 temp = MatrixTranslate(MoveX, MoveY, MoveZ);
	for (int i = 0; i < VERTEXNUM; i++)
	{
		Vertex[i] = Multiple(Vertex[i], temp);
	}
	Centroid = GetTriangleCenter(Vertex);
}

//������ ������
void Player::EquationLine(float* PosX, float* PosY, Matrix1x4 data1, Matrix1x4 data2)
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
//������ �̾� ������ �׸��� �Լ�
void Player::DrowLine(HDC _hdc, float* PosX, float* PosY, Matrix1x4 data1, Matrix1x4 data2,bool Iscrash)
{
	int a = data1.m[0][0] - data2.m[0][0];
	int b = data1.m[0][1] - data2.m[0][1];

	
	if (Iscrash) //�浹�� ���Ͱ������� ���������� ����
	{
		Color = 255;
	}
	else
	{
		Color = 0;
	}

	if (abs(a) > abs(b))
	{
		EquationLine(PosX, PosY, data1, data2);
		for (int i = 0; i < abs(a); i++)//���
		{
			
			SetPixel(_hdc, PosX[i], PosY[i], RGB(Color, 0, 0));
		}
	}
	else if (abs(b) > abs(a))
	{
		EquationLine(PosX, PosY, data1, data2);
		for (int i = 0; i < abs(b); i++)//���
		{
			SetPixel(_hdc, PosX[i], PosY[i], RGB(Color, 0, 0));
		}
	}
}
//�ﰢ���� �����߽��� ������ �׸��� �Լ�
void Player::DrowTriangleCenter(HDC _hdc)
{
	HBRUSH Brush, oBrush;
	
	Brush = CreateSolidBrush(RGB(255, 0, 0));
	oBrush = (HBRUSH)SelectObject(_hdc, Brush);
	Ellipse(_hdc, Centroid.Get_X() - RADIUS, Centroid.Get_Y() - RADIUS, Centroid.Get_X() + RADIUS, Centroid.Get_Y() + RADIUS);
	SelectObject(_hdc, oBrush);
	DeleteObject(Brush);

}
//�ﰢ�� �׸����Լ�
void Player::DrowTriangle(HDC _hdc, float* PosX, float* PosY,bool Iscrash)
{
	DrowLine(_hdc, PosX, PosY, Vertex[0], Vertex[1], Iscrash);
	DrowLine(_hdc, PosX, PosY, Vertex[0], Vertex[2], Iscrash);
	DrowLine(_hdc, PosX, PosY, Vertex[1], Vertex[2], Iscrash);
	DrowTriangleCenter(_hdc);
}


float Player::GetRadius()
{
	float temp;
	float Radius=0;
	float x, y;
	for (int i = 0; i < 3; i++)
	{
		x = Centroid.Get_X() - Vertex[i].m[0][0];
		y = Centroid.Get_Y() - Vertex[i].m[0][1];
		temp = sqrt((float)(x * x) + (y * y));

		if (Radius < temp)
		{
			Radius = temp;
		}
	}
	return Radius;
}
