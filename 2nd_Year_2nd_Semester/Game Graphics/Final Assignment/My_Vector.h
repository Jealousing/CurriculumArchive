#pragma once
#include <iostream>

using namespace std;

class My_Vector//���� Ŭ��������
{
private:

	float x, y, z;
	float size;

public:
	My_Vector()//������
	{
		x = 0; y = 0; z = 0; size = 0;
	}
	My_Vector(float _x, float _y,float _z)
	{
		x = _x;
		y = _y;
		z = _z;
		size = 0;
	}
	My_Vector(const My_Vector& copy)
	{
		this->x = copy.x;
		this->y = copy.y;
		this->z = copy.z;
		this->size = 0;
	}
	~My_Vector()//�Ҹ���
	{

	}

	float Get_X()
	{
		return this->x;
	}
	float Get_Y()
	{
		return this->y;
	}
	float Get_Z()
	{
		return this->z;
	}

	void Set_X(float setx)
	{
		x = setx;
	}
	void Set_Y(float sety)
	{
		y = sety;
	}
	void Set_Z(float setz)
	{
		y = setz;
	}
	void SetPos(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	void Size()
	{
		size = x * x + y * y+z*z;
	}
	
	//������
	My_Vector operator+(const My_Vector v)const
	{
		return My_Vector(x + v.x, y + v.y,z+v.z);
	}
	//������
	My_Vector operator-(const My_Vector v)const
	{
		return My_Vector(x - v.x, y - v.y,z-v.z);
	}
	//��Į���
	My_Vector operator*(float num)const
	{
		return My_Vector(x *num, y * num ,z* num);
	}
	//������
	bool operator==(const My_Vector v)
	{
		if (this->x == v.x && this->y == v.y)
		{
			return true;
		}
		else
			false;
	}
	//����ȭ
	void NormalizeVec()
	{
		float leng = sqrtf(x * x + y * y + z * z);

		x /= leng;
		y /= leng;
		z /= leng;
	}
	//����
	My_Vector CrossVec(const My_Vector& vec)const
	{
		return My_Vector(y * vec.z - z * vec.y,z*vec.x-x*vec.z,x*vec.y-y*vec.x );
	}

};

