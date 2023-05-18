#include "Game2D.h"
#include "Examples/PrimitivesGallery.h"
#include "RandomNumberGenerator.h"
#include <vector>
#include <memory>
#include <tuple>
#include "ShapeObject.h"
//스마트포인트 때문에 늘어난 해더파일들...
#include "Triangle.h"
#include "Circle.h"
#include "Box.h"
#include "Star.h"
#include "Pentagon.h"
using namespace std;


namespace SeoDongJu
{
	//함수선언 부분
	void DrawRun();//진행함수
	RGB colorPick(string _pick);//색깔 선택 함수
	//정보를 입력받는 함수 (return을 한개만 받는것이 아닌 여러개를 받을 수 있는 함수 (tuple)
	tuple<const RGB, const float, const float, const float, const  float> setting2();//정보 5개
	tuple<const RGB, const float, const float, const float> setting1();//정보 4개
	

	class Figure : public Game2D
	{
	public:
		//스마트 포인터적용한 부분과  Factory pattern을 적용한 부분이 서로 겹치기때문에
		//중복실행됩니다. 기본 도형 메뉴에서만 중복구현하게 했습니다.
		//factory pattern
		vector<shapeObject*> my_objs;
		//스마트포인터
		vector<shared_ptr<shapeObject>> my_obj2;
		Figure(int mainmenu,int submenu) : Game2D()
		{//생성자
			if (mainmenu == 1)//메뉴선택 메인1번메뉴
			{
				switch (submenu)//서브메뉴 선택
				{
				case 1://삼각형
				{
					//정보 입력받기
					tuple<const RGB, const float, const float, const float> my_setting = setting1();
					//factory pattern
					my_objs.push_back(shapeObject::makeTriangle(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting)));
					//스마트 포인터 구현 방식
					my_obj2.push_back(shared_ptr<Triangle>(new Triangle(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting))));
					break;
				}
				case 2://직사각형
				{
					//정보 입력받기
					tuple<const RGB, const float, const float, const float, const  float> my_setting = setting2();
					//factory pattern
					my_objs.push_back(shapeObject::makeBox(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting), get<4>(my_setting)));
					//스마트 포인터 구현
					my_obj2.push_back(shared_ptr<Box>(new Box(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting), get<4>(my_setting))));
					break;
				}
				case 3:
				{
					//정보 입력받기
					tuple<const RGB, const float, const float, const float> my_setting = setting1();
					//factory pattern
					my_objs.push_back(shapeObject::makeCircle(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting)));
					//스마트 포인터 구현 방식
					my_obj2.push_back(shared_ptr<Circle>(new Circle(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting))));
					break;
				}
				case 4:
				{
					//정보 입력받기
					tuple<const RGB, const float, const float, const float, const  float> my_setting = setting2();
					//factory pattern
					my_objs.push_back(shapeObject::makestar(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting), get<4>(my_setting)));
					//스마트 포인터 구현
					my_obj2.push_back(shared_ptr<Star>(new Star(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting), get<4>(my_setting))));
					break;
				}
				case 5:
				{
					//정보 입력받기
					tuple<const RGB, const float, const float, const float> my_setting = setting1();
					//factory pattern
					my_objs.push_back(shapeObject::makePentagon(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting)));
					//스마트 포인터 구현 방식
					my_obj2.push_back(shared_ptr<Pentagon>(new Pentagon(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting))));
					break;
				}
				default:
					break;
				}
			}
			else if (mainmenu == 2)//완성형 도형선택
			{
				switch (submenu)//모양 선택
				{
				case 1:
				{
					//집 그림
					my_objs.push_back(shapeObject::makeBox(Colors::brown, vec2{ -0.5f,-0.6f }, 0.8f, 0.5f));
					my_objs.push_back(shapeObject::makeTriangle(Colors::red, vec2{ -0.5f,-0.1f }, 0.8f));
					my_objs.push_back(shapeObject::makeBox(Colors::black, vec2{ -0.3f,-0.7f }, 0.2f, 0.3f));
					my_objs.push_back(shapeObject::makeBox(Colors::skyblue, vec2{ -0.68f,-0.6f }, 0.3f, 0.2f));
					my_objs.push_back(shapeObject::makeCircle(Colors::white, vec2{ -0.37f,-0.72f }, 0.01f));
					break;
				}
				case 2:
				{
					//사과나무 그림
					my_objs.push_back(shapeObject::makeBox(Colors::brown, vec2{ 0.7f,-0.4f }, 0.2f, 1.0f));
					my_objs.push_back(shapeObject::makeCircle(Colors::green, vec2{ 0.7f,0.01f }, 0.3f));
					my_objs.push_back(shapeObject::makeCircle(Colors::green, vec2{ 0.95f,0.15f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::green, vec2{ 1.0f,0.00f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::green, vec2{ 0.95f,-0.15f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::green, vec2{ 0.43f,0.15f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::green, vec2{ 0.38f,0.00f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::green, vec2{ 0.43f,-0.15f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::green, vec2{ 0.6f,0.27f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::green, vec2{ 0.8f,0.27f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::red, vec2{ 0.5f,-0.163f }, 0.05f));
					my_objs.push_back(shapeObject::makeCircle(Colors::red, vec2{ 0.8f,-0.163f }, 0.05f));
					my_objs.push_back(shapeObject::makeCircle(Colors::red, vec2{ 0.65f,0.08f }, 0.05f));
					my_objs.push_back(shapeObject::makeCircle(Colors::red, vec2{ 0.85f,0.08f }, 0.05f));
					break;
				}
				case 3:
				{//하늘 (구름+해)
					my_objs.push_back(shapeObject::makeCircle(Colors::yellow, vec2{ -1.5f,1.3f }, 1.0f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.85f,0.6f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.85f,0.7f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.8f,0.75f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.8f,0.58f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.75f,0.58f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.75f,0.75f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.70f,0.58f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.70f,0.75f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.67f,0.55f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.67f,0.77f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.62f,0.58f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.62f,0.75f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.57f,0.58f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.57f,0.75f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.52f,0.58f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.52f,0.75f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.47f,0.6f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.47f,0.7f }, 0.1f));
					break;
				}
				case 4:
				{
					//합치기
					//집
					my_objs.push_back(shapeObject::makeBox(Colors::brown, vec2{ -0.5f,-0.6f }, 0.8f, 0.5f));
					my_objs.push_back(shapeObject::makeTriangle(Colors::red, vec2{ -0.5f,-0.1f }, 0.8f));
					my_objs.push_back(shapeObject::makeBox(Colors::black, vec2{ -0.3f,-0.7f }, 0.2f, 0.3f));
					my_objs.push_back(shapeObject::makeBox(Colors::skyblue, vec2{ -0.68f,-0.6f }, 0.3f, 0.2f));
					my_objs.push_back(shapeObject::makeCircle(Colors::white, vec2{ -0.37f,-0.72f }, 0.01f));
					//나무
					my_objs.push_back(shapeObject::makeBox(Colors::brown, vec2{ 0.7f,-0.4f }, 0.2f, 1.0f));
					my_objs.push_back(shapeObject::makeCircle(Colors::green, vec2{ 0.7f,0.01f }, 0.3f));
					my_objs.push_back(shapeObject::makeCircle(Colors::green, vec2{ 0.95f,0.15f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::green, vec2{ 1.0f,0.00f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::green, vec2{ 0.95f,-0.15f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::green, vec2{ 0.43f,0.15f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::green, vec2{ 0.38f,0.00f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::green, vec2{ 0.43f,-0.15f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::green, vec2{ 0.6f,0.27f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::green, vec2{ 0.8f,0.27f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::red, vec2{ 0.5f,-0.163f }, 0.05f));
					my_objs.push_back(shapeObject::makeCircle(Colors::red, vec2{ 0.8f,-0.163f }, 0.05f));
					my_objs.push_back(shapeObject::makeCircle(Colors::red, vec2{ 0.65f,0.08f }, 0.05f));
					my_objs.push_back(shapeObject::makeCircle(Colors::red, vec2{ 0.85f,0.08f }, 0.05f));
					//하늘배경
					my_objs.push_back(shapeObject::makeCircle(Colors::yellow, vec2{ -1.5f,1.3f }, 1.0f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.85f,0.6f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.85f,0.7f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.8f,0.75f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.8f,0.58f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.75f,0.58f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.75f,0.75f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.70f,0.58f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.70f,0.75f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.67f,0.55f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.67f,0.77f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.62f,0.58f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.62f,0.75f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.57f,0.58f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.57f,0.75f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.52f,0.58f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.52f,0.75f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.47f,0.6f }, 0.1f));
					my_objs.push_back(shapeObject::makeCircle(Colors::skyblue, vec2{ 0.47f,0.7f }, 0.1f));
					break;
				}
				default:
					break;
				}
			}
		}
		~Figure()
		{//소멸자 메모리공간 비움!
			for (const auto& obj : my_objs)
			{
				delete obj;
			}
		}

		void update() override
		{
			//factory pattern 그리기
			for (const auto& obj : my_objs)
			{
				obj->draw();
			}
			//스마트포인터 그리기
			for (const auto& obj2 : my_obj2)
			{
				obj2->draw();
			}
		}
	};
	
}

