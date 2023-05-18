#pragma once
#include "ShapeObject.h"

namespace SeoDongJu
{
	class Triangle : public shapeObject //shapeObject를 상속받음.
	{
	public:
		float size;//크기
		Triangle(const RGB& _color, const vec2& _pos, const float& _size)
		{//생성자
			init(_color, _pos, _size);
		}
		~Triangle()
		{//소멸자
			printf("세모 소멸됨\n");
		}
		void init(const RGB& _color, const vec2& _pos, const float& _size)
		{
			shapeObject::init(_color, _pos);//다른 클레스와 공통인 부분은 상속받는 클레스에서 처리.
			//다른것은 따로 처리
			size = _size;
		}
		void drawShape()
		{//OpenGL 그리기 부분
			drawFilledTriangle(color, size);
		}
	};
}

