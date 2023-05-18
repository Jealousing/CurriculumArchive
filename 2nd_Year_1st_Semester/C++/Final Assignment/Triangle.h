#pragma once
#include "ShapeObject.h"

namespace SeoDongJu
{
	class Triangle : public shapeObject //shapeObject�� ��ӹ���.
	{
	public:
		float size;//ũ��
		Triangle(const RGB& _color, const vec2& _pos, const float& _size)
		{//������
			init(_color, _pos, _size);
		}
		~Triangle()
		{//�Ҹ���
			printf("���� �Ҹ��\n");
		}
		void init(const RGB& _color, const vec2& _pos, const float& _size)
		{
			shapeObject::init(_color, _pos);//�ٸ� Ŭ������ ������ �κ��� ��ӹ޴� Ŭ�������� ó��.
			//�ٸ����� ���� ó��
			size = _size;
		}
		void drawShape()
		{//OpenGL �׸��� �κ�
			drawFilledTriangle(color, size);
		}
	};
}

