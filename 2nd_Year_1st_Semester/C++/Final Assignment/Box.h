#pragma once
#include "ShapeObject.h"
namespace SeoDongJu
{
	class Box :public shapeObject//shapeObject�� ��ӹ���.
	{
	public:
		float width, height; //���μ��� 
		Box(const RGB& _color, const vec2& _pos, const float& _width, const float& _height)
		{//������
			init(_color, _pos, _width, _height);
		}
		~Box()
		{//�Ҹ���
			printf("�׸�Ҹ��\n");
		}
		void init(const RGB& _color, const vec2 &_pos,const float &_width,const float &_height)
		{
			shapeObject::init(_color, _pos);//�ٸ� Ŭ������ ������ �κ��� ��ӹ޴� Ŭ�������� ó��.
			//�ٸ����� ���� ó��
			width = _width;
			height = _height;
		}

		void drawShape()
		{//OpenGL �׸��� �κ�
			drawFilledBox(color, width, height);
		}
	};
}