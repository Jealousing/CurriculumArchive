#include "ShapeObject.h"
#include "Triangle.h"
#include "Circle.h"
#include "Box.h"
#include "Star.h"
#include "Pentagon.h"
#include <memory>

using namespace std;

namespace SeoDongJu
{//정의부분 도형그리기!
	shapeObject* shapeObject::makeTriangle(const RGB& _color, const vec2& _pos, const float& _size)
	{
		return new  Triangle(_color, _pos, _size);
	}
	shapeObject* shapeObject::makeCircle(const RGB& _color, const vec2& _pos, const float& _size)
	{
		return new Circle(_color, _pos, _size);
	}
	shapeObject* shapeObject::makeBox(const RGB& _color, const vec2& _pos, const float& _width, const float& _height)
	{
		return new Box(_color, _pos, _width, _height);
	}
	shapeObject* shapeObject::makestar(const RGB& _color, const vec2& _pos, const float& _width, const float& _height)
	{
		return new Star(_color, _pos, _width, _height);
	}
	shapeObject* shapeObject::makePentagon(const RGB& _color, const vec2& _pos, const float& _size)
	{
		return new Pentagon(_color, _pos, _size);
	}
}