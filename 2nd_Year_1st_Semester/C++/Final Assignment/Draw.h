#include "Game2D.h"
#include "Examples/PrimitivesGallery.h"
#include "RandomNumberGenerator.h"
#include <vector>
#include <memory>
#include <tuple>
#include "ShapeObject.h"
//����Ʈ����Ʈ ������ �þ �ش����ϵ�...
#include "Triangle.h"
#include "Circle.h"
#include "Box.h"
#include "Star.h"
#include "Pentagon.h"
using namespace std;


namespace SeoDongJu
{
	//�Լ����� �κ�
	void DrawRun();//�����Լ�
	RGB colorPick(string _pick);//���� ���� �Լ�
	//������ �Է¹޴� �Լ� (return�� �Ѱ��� �޴°��� �ƴ� �������� ���� �� �ִ� �Լ� (tuple)
	tuple<const RGB, const float, const float, const float, const  float> setting2();//���� 5��
	tuple<const RGB, const float, const float, const float> setting1();//���� 4��
	

	class Figure : public Game2D
	{
	public:
		//����Ʈ ������������ �κа�  Factory pattern�� ������ �κ��� ���� ��ġ�⶧����
		//�ߺ�����˴ϴ�. �⺻ ���� �޴������� �ߺ������ϰ� �߽��ϴ�.
		//factory pattern
		vector<shapeObject*> my_objs;
		//����Ʈ������
		vector<shared_ptr<shapeObject>> my_obj2;
		Figure(int mainmenu,int submenu) : Game2D()
		{//������
			if (mainmenu == 1)//�޴����� ����1���޴�
			{
				switch (submenu)//����޴� ����
				{
				case 1://�ﰢ��
				{
					//���� �Է¹ޱ�
					tuple<const RGB, const float, const float, const float> my_setting = setting1();
					//factory pattern
					my_objs.push_back(shapeObject::makeTriangle(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting)));
					//����Ʈ ������ ���� ���
					my_obj2.push_back(shared_ptr<Triangle>(new Triangle(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting))));
					break;
				}
				case 2://���簢��
				{
					//���� �Է¹ޱ�
					tuple<const RGB, const float, const float, const float, const  float> my_setting = setting2();
					//factory pattern
					my_objs.push_back(shapeObject::makeBox(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting), get<4>(my_setting)));
					//����Ʈ ������ ����
					my_obj2.push_back(shared_ptr<Box>(new Box(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting), get<4>(my_setting))));
					break;
				}
				case 3:
				{
					//���� �Է¹ޱ�
					tuple<const RGB, const float, const float, const float> my_setting = setting1();
					//factory pattern
					my_objs.push_back(shapeObject::makeCircle(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting)));
					//����Ʈ ������ ���� ���
					my_obj2.push_back(shared_ptr<Circle>(new Circle(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting))));
					break;
				}
				case 4:
				{
					//���� �Է¹ޱ�
					tuple<const RGB, const float, const float, const float, const  float> my_setting = setting2();
					//factory pattern
					my_objs.push_back(shapeObject::makestar(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting), get<4>(my_setting)));
					//����Ʈ ������ ����
					my_obj2.push_back(shared_ptr<Star>(new Star(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting), get<4>(my_setting))));
					break;
				}
				case 5:
				{
					//���� �Է¹ޱ�
					tuple<const RGB, const float, const float, const float> my_setting = setting1();
					//factory pattern
					my_objs.push_back(shapeObject::makePentagon(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting)));
					//����Ʈ ������ ���� ���
					my_obj2.push_back(shared_ptr<Pentagon>(new Pentagon(get<0>(my_setting),
						vec2{ get<1>(my_setting),get<2>(my_setting) }, get<3>(my_setting))));
					break;
				}
				default:
					break;
				}
			}
			else if (mainmenu == 2)//�ϼ��� ��������
			{
				switch (submenu)//��� ����
				{
				case 1:
				{
					//�� �׸�
					my_objs.push_back(shapeObject::makeBox(Colors::brown, vec2{ -0.5f,-0.6f }, 0.8f, 0.5f));
					my_objs.push_back(shapeObject::makeTriangle(Colors::red, vec2{ -0.5f,-0.1f }, 0.8f));
					my_objs.push_back(shapeObject::makeBox(Colors::black, vec2{ -0.3f,-0.7f }, 0.2f, 0.3f));
					my_objs.push_back(shapeObject::makeBox(Colors::skyblue, vec2{ -0.68f,-0.6f }, 0.3f, 0.2f));
					my_objs.push_back(shapeObject::makeCircle(Colors::white, vec2{ -0.37f,-0.72f }, 0.01f));
					break;
				}
				case 2:
				{
					//������� �׸�
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
				{//�ϴ� (����+��)
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
					//��ġ��
					//��
					my_objs.push_back(shapeObject::makeBox(Colors::brown, vec2{ -0.5f,-0.6f }, 0.8f, 0.5f));
					my_objs.push_back(shapeObject::makeTriangle(Colors::red, vec2{ -0.5f,-0.1f }, 0.8f));
					my_objs.push_back(shapeObject::makeBox(Colors::black, vec2{ -0.3f,-0.7f }, 0.2f, 0.3f));
					my_objs.push_back(shapeObject::makeBox(Colors::skyblue, vec2{ -0.68f,-0.6f }, 0.3f, 0.2f));
					my_objs.push_back(shapeObject::makeCircle(Colors::white, vec2{ -0.37f,-0.72f }, 0.01f));
					//����
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
					//�ϴù��
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
		{//�Ҹ��� �޸𸮰��� ���!
			for (const auto& obj : my_objs)
			{
				delete obj;
			}
		}

		void update() override
		{
			//factory pattern �׸���
			for (const auto& obj : my_objs)
			{
				obj->draw();
			}
			//����Ʈ������ �׸���
			for (const auto& obj2 : my_obj2)
			{
				obj2->draw();
			}
		}
	};
	
}

