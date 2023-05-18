#pragma once
#include "ShapeObject.h"
namespace SeoDongJu
{
	class Star :public shapeObject//shapeObject를 상속받음.
	{
	public:
		float width, height; //가로세로 
		Star(const RGB& _color, const vec2& _pos, const float& _width, const float& _height)
		{//생성자
			init(_color, _pos, _width, _height);
		}
		~Star()
		{//소멸자
			printf("별소멸\n");
		}
		void init(const RGB& _color, const vec2& _pos, const float& _width, const float& _height)
		{
			shapeObject::init(_color, _pos);//다른 클레스와 공통인 부분은 상속받는 클레스에서 처리.
			//다른것은 따로 처리
			width = _width;
			height = _height;
		}

		void drawShape()
		{//OpenGL 그리기 부분
			drawFilledStar(color, width, height);
		}
	};
}