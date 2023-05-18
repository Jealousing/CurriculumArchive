#pragma once
#include <string>
#include "Game2D.h"

namespace SeoDongJu
{
	class shapeObject
	{
	public:
		vec2 pos;//좌표
		RGB color;//색상
		virtual ~shapeObject() {}//소멸자
		void init(const RGB & _color,const vec2 &_pos)
		{//색이랑 좌표설정
			color = _color;
			pos = _pos;
		}
		virtual void drawShape(){}//가상함수 virtual을 사용하여 서브클레스에서 정의된걸로 사용
		void draw()//중복되는 부분을 한번에 처리
		{
			beginTransformation();
			{
				translate(pos);//위치지정
				drawShape();//그리기
			}
			endTransformation();
		}

		//도형 만들기
		//공통부분 묶기
		static shapeObject* makeTriangle(const RGB& _color, const vec2& _pos, const float& _size);
		static shapeObject* makeCircle(const RGB& _color, const vec2& _pos, const float& _size);
		static shapeObject* makePentagon(const RGB& _color, const vec2& _pos, const float& _size);
		static shapeObject* makeBox(const RGB& _color, const vec2& _pos, const float& _width, const float& _height);
		static shapeObject* makestar(const RGB& _color, const vec2& _pos, const float& _width, const float& _height);
		
	};
}