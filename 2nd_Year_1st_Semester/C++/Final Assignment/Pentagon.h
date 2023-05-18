#pragma once
#include "ShapeObject.h"
namespace SeoDongJu
{
	class Pentagon :public shapeObject//shapeObject�� ��ӹ���.
	{
	public:
		float size;//ũ��
		Pentagon(const RGB& _color, const vec2& _pos, const float& _size)
		{//������
			init(_color, _pos, _size);
		}
		~Pentagon()//�Ҹ���
		{
			printf("������ �Ҹ��\n");
		}
		void init(const RGB& _color, const vec2& _pos, const float& _size)
		{
			shapeObject::init(_color, _pos);//�ٸ� Ŭ������ ������ �κ��� ��ӹ޴� Ŭ�������� ó��.
			//�ٸ����� ���� ó��
			size = _size;
		}
		void drawShape()
		{//OpenGL �׸��� �κ�
			drawFilledPentagon(color, size);
		}
	};
}