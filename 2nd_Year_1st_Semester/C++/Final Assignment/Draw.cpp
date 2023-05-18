#include "Draw.h"

namespace SeoDongJu
{
	void DrawRun()//그리기 위한 메뉴 호출 및 클래스 호출
	{
		bool end = true;//종료를 하기위한 변수
		bool menuPick = true;//메인메뉴 선택시 반복을 위한 변수
		while (end)
		{
			cout << "다음 중에서 원하는 메뉴를 선택하시오" << endl;
			cout << "ex)1.기본도형 2.완성된 도형 3.종료" << endl << "입력: ";
			int menu;//메인메뉴 선택
			cin >> menu;
			switch (menu)
			{
			case 1://기본도형
				menuPick = true;
				while (menuPick)
				{
					cout << "다음 도형 중에서 원하는 메뉴를 선택하시오" << endl;
					cout << "1.삼각형 2.사각형 3.원 4.별 5.오각형 6.이전으로" << endl << "입력: ";
					int submenu;//서브메뉴 선택
					cin >> submenu;
					switch (submenu)
					{//위치 사이즈 입력받기
					case 1://선택시 생성자호출
						cout << "선택한 도형은 삼각형입니다." << endl;
						Figure(menu,submenu).run();
						break;
					case 2:
						cout << "선택한 도형은 사각형입니다.." << endl;
						Figure(menu,submenu).run();
						break;
					case 3:
						cout << "선택한 도형은 원입니다." << endl;
						Figure(menu,submenu).run();
						break;
					case 4:
						cout << "선택한 도형은 별입니다." << endl;
						cout << "가로세로 입력할때 가로의  길이가 더 커야 정상적으로 출력됩니다." << endl;
						Figure(menu,submenu).run();
						break;
					case 5:
						cout << "선택한 도형은 오각형입니다." << endl;
						Figure(menu,submenu).run();
						break;
					case 6://이전메뉴선택화면으로 돌아가기.
						menuPick = false;
						break;
					default:
						break;
					}
				}
				break;
			case 2://완성된 도형
				menuPick = true;
				while (menuPick)
				{
					int submenu;
					cout << "다음 중에서 원하는 메뉴를 선택하시오" << endl;
					cout << "1.집 2.사과나무 3.하늘풍경 4.집+사과나무 풍경 5.이전으로" << endl << "입력 :";
					cin >> submenu;//서브메뉴선택
					switch (submenu)
					{//선택에따른 생성자 호출
					case 1:
						cout << "선택한 완성형 도형은 집입니다." << endl;
						Figure(menu, submenu).run();
						break;
					case 2:
						cout << "선택한 완성형 도형은 사과나무입니다." << endl;
						Figure(menu, submenu).run();
						break;
					case 3:
						cout << "선택한 것은 하늘 풍경입니다." << endl;
						Figure(menu, submenu).run();
						break;
					case 4:
						cout << "선택한 것은 집+사과나무+하늘풍경입니다." << endl;
						Figure(menu, submenu).run();
						break;
					case 5://이전메뉴로
						menuPick = false;
						break;
					default:
						break;
					}
				}
				break;
			case 3://종료
				end = false;
				break;

			}
		}


	}

	//원하는 정보를 입력하고 return 받기위한 함수 사용
	tuple < const RGB, const float, const float, const float, const float > setting2()//정보5개
	{
		//색지정
		cout <<"원하는 색을 입력하시오:" <<endl;
		cout << "red, green, blue, skyblue, gray, yellow" << endl;
		cout << "olive, black, white, gold, silver" << endl;
		cout << "입력 :";
		string pick;
		cin >> pick;
		RGB colorpick=colorPick(pick);

		//vec2의 좌표 입력을 위한 float형 좌표 두개 입력받기.
		cout << "x 좌표를 입력하세요 ex) 0.5" << endl << "입력 :";
		float posx;
		cin >> posx;
		cout << "y 좌표를 입력하세요 ex) 0.5" << endl << "입력 :";
		float posy;
		cin >> posy;

		//길이 입력 받기
		cout << "가로의 길이를 입력하시오 ex) 0.5" << endl << "입력 :";
		float width;
		cin >> width;
		cout << "세로의 길이를 입력하시오 ex) 0.5" << endl << "입력 :";
		float height;
		cin >> height;
		//리턴으로 5가지의 정보를 전달
		return make_tuple(colorpick,  posx, posy, width, height);
	}
	//위와 동일하지만 입력정보는 4개
	tuple<const RGB, const float, const float, const float> setting1()
	{
		//색지정
		cout << "원하는 색을 입력하시오:" << endl;
		cout << "red, green, blue, skyblud, gray, yellow" << endl;
		cout << "olive, black, white, gold, silver" << endl;
		cout << "입력 :";
		string pick;
		cin >> pick;
		RGB colorpick = colorPick(pick);

		//vec2의 좌표 입력을 위한 float형 좌표 두개 입력받기.
		cout << "x 좌표를 입력하세요 ex) 0.5" << endl << "입력 :";
		float posx;
		cin >> posx;
		cout << "y 좌표를 입력하세요 ex) 0.5" << endl << "입력 :";
		float posy;
		cin >> posy;

		//길이대신 크기입력받기
		cout << "크기를 입력하시오 ex) 0.5" << endl << "입력 :";
		float size;
		cin >> size;
		//리턴으로 4가지의 정보를 전달
		return make_tuple(colorpick, posx, posy, size);
	}

	//색 지정을 위함
	RGB colorPick(string _pick)
	{//컬러설정!
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
		//아무것도없으면 블랙
		return Colors::black;
	}
}
