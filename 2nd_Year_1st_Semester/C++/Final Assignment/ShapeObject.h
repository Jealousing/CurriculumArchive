#pragma once
#include <string>
#include "Game2D.h"

namespace SeoDongJu
{
	class shapeObject
	{
	public:
		vec2 pos;//��ǥ
		RGB color;//����
		virtual ~shapeObject() {}//�Ҹ���
		void init(const RGB & _color,const vec2 &_pos)
		{//���̶� ��ǥ����
			color = _color;
			pos = _pos;
		}
		virtual void drawShape(){}//�����Լ� virtual�� ����Ͽ� ����Ŭ�������� ���ǵȰɷ� ���
		void draw()//�ߺ��Ǵ� �κ��� �ѹ��� ó��
		{
			beginTransformation();
			{
				translate(pos);//��ġ����
				drawShape();//�׸���
			}
			endTransformation();
		}

		//���� �����
		//����κ� ����
		static shapeObject* makeTriangle(const RGB& _color, const vec2& _pos, const float& _size);
		static shapeObject* makeCircle(const RGB& _color, const vec2& _pos, const float& _size);
		static shapeObject* makePentagon(const RGB& _color, const vec2& _pos, const float& _size);
		static shapeObject* makeBox(const RGB& _color, const vec2& _pos, const float& _width, const float& _height);
		static shapeObject* makestar(const RGB& _color, const vec2& _pos, const float& _width, const float& _height);
		
	};
}