#include "Draw.h"

namespace SeoDongJu
{
	void DrawRun()//�׸��� ���� �޴� ȣ�� �� Ŭ���� ȣ��
	{
		bool end = true;//���Ḧ �ϱ����� ����
		bool menuPick = true;//���θ޴� ���ý� �ݺ��� ���� ����
		while (end)
		{
			cout << "���� �߿��� ���ϴ� �޴��� �����Ͻÿ�" << endl;
			cout << "ex)1.�⺻���� 2.�ϼ��� ���� 3.����" << endl << "�Է�: ";
			int menu;//���θ޴� ����
			cin >> menu;
			switch (menu)
			{
			case 1://�⺻����
				menuPick = true;
				while (menuPick)
				{
					cout << "���� ���� �߿��� ���ϴ� �޴��� �����Ͻÿ�" << endl;
					cout << "1.�ﰢ�� 2.�簢�� 3.�� 4.�� 5.������ 6.��������" << endl << "�Է�: ";
					int submenu;//����޴� ����
					cin >> submenu;
					switch (submenu)
					{//��ġ ������ �Է¹ޱ�
					case 1://���ý� ������ȣ��
						cout << "������ ������ �ﰢ���Դϴ�." << endl;
						Figure(menu,submenu).run();
						break;
					case 2:
						cout << "������ ������ �簢���Դϴ�.." << endl;
						Figure(menu,submenu).run();
						break;
					case 3:
						cout << "������ ������ ���Դϴ�." << endl;
						Figure(menu,submenu).run();
						break;
					case 4:
						cout << "������ ������ ���Դϴ�." << endl;
						cout << "���μ��� �Է��Ҷ� ������  ���̰� �� Ŀ�� ���������� ��µ˴ϴ�." << endl;
						Figure(menu,submenu).run();
						break;
					case 5:
						cout << "������ ������ �������Դϴ�." << endl;
						Figure(menu,submenu).run();
						break;
					case 6://�����޴�����ȭ������ ���ư���.
						menuPick = false;
						break;
					default:
						break;
					}
				}
				break;
			case 2://�ϼ��� ����
				menuPick = true;
				while (menuPick)
				{
					int submenu;
					cout << "���� �߿��� ���ϴ� �޴��� �����Ͻÿ�" << endl;
					cout << "1.�� 2.������� 3.�ϴ�ǳ�� 4.��+������� ǳ�� 5.��������" << endl << "�Է� :";
					cin >> submenu;//����޴�����
					switch (submenu)
					{//���ÿ����� ������ ȣ��
					case 1:
						cout << "������ �ϼ��� ������ ���Դϴ�." << endl;
						Figure(menu, submenu).run();
						break;
					case 2:
						cout << "������ �ϼ��� ������ ��������Դϴ�." << endl;
						Figure(menu, submenu).run();
						break;
					case 3:
						cout << "������ ���� �ϴ� ǳ���Դϴ�." << endl;
						Figure(menu, submenu).run();
						break;
					case 4:
						cout << "������ ���� ��+�������+�ϴ�ǳ���Դϴ�." << endl;
						Figure(menu, submenu).run();
						break;
					case 5://�����޴���
						menuPick = false;
						break;
					default:
						break;
					}
				}
				break;
			case 3://����
				end = false;
				break;

			}
		}


	}

	//���ϴ� ������ �Է��ϰ� return �ޱ����� �Լ� ���
	tuple < const RGB, const float, const float, const float, const float > setting2()//����5��
	{
		//������
		cout <<"���ϴ� ���� �Է��Ͻÿ�:" <<endl;
		cout << "red, green, blue, skyblue, gray, yellow" << endl;
		cout << "olive, black, white, gold, silver" << endl;
		cout << "�Է� :";
		string pick;
		cin >> pick;
		RGB colorpick=colorPick(pick);

		//vec2�� ��ǥ �Է��� ���� float�� ��ǥ �ΰ� �Է¹ޱ�.
		cout << "x ��ǥ�� �Է��ϼ��� ex) 0.5" << endl << "�Է� :";
		float posx;
		cin >> posx;
		cout << "y ��ǥ�� �Է��ϼ��� ex) 0.5" << endl << "�Է� :";
		float posy;
		cin >> posy;

		//���� �Է� �ޱ�
		cout << "������ ���̸� �Է��Ͻÿ� ex) 0.5" << endl << "�Է� :";
		float width;
		cin >> width;
		cout << "������ ���̸� �Է��Ͻÿ� ex) 0.5" << endl << "�Է� :";
		float height;
		cin >> height;
		//�������� 5������ ������ ����
		return make_tuple(colorpick,  posx, posy, width, height);
	}
	//���� ���������� �Է������� 4��
	tuple<const RGB, const float, const float, const float> setting1()
	{
		//������
		cout << "���ϴ� ���� �Է��Ͻÿ�:" << endl;
		cout << "red, green, blue, skyblud, gray, yellow" << endl;
		cout << "olive, black, white, gold, silver" << endl;
		cout << "�Է� :";
		string pick;
		cin >> pick;
		RGB colorpick = colorPick(pick);

		//vec2�� ��ǥ �Է��� ���� float�� ��ǥ �ΰ� �Է¹ޱ�.
		cout << "x ��ǥ�� �Է��ϼ��� ex) 0.5" << endl << "�Է� :";
		float posx;
		cin >> posx;
		cout << "y ��ǥ�� �Է��ϼ��� ex) 0.5" << endl << "�Է� :";
		float posy;
		cin >> posy;

		//���̴�� ũ���Է¹ޱ�
		cout << "ũ�⸦ �Է��Ͻÿ� ex) 0.5" << endl << "�Է� :";
		float size;
		cin >> size;
		//�������� 4������ ������ ����
		return make_tuple(colorpick, posx, posy, size);
	}

	//�� ������ ����
	RGB colorPick(string _pick)
	{//�÷�����!
		if (_pick == "red")
			return Colors::red;
		if (_pick == "green")
			return Colors::green;
		if (_pick == "blue")
			return Colors::blue;
		if (_pick == "skyblue")
			return Colors::skyblue;
		if (_pick == "gray")
			return Colors::gray;
		if (_pick == "yellow")
			return Colors::yellow;
		if (_pick == "olive")
			return Colors::olive;
		if (_pick == "black")
			return Colors::black;
		if (_pick == "white")
			return Colors::white;
		if (_pick == "gold")
			return Colors::gold;
		if (_pick == "silver")
			return Colors::silver;
		//�ƹ��͵������� ��
		return Colors::black;
	}
}
