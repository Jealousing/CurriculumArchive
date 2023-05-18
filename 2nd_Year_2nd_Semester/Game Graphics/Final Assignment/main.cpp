#include <windows.h>			//윈도우 헤더 파일
#include "My_Vertex.h"

//화면크기
#define WNDSIZE_WIDTH 1980
#define WNDSIZE_HEIGHT 1080
//도형 움직임 횟수
#define MOVENUM 5
//키보드 이동사이즈
#define MOVESPEED 10
//축사이즈
#define LINESIZE 2048
//점의 갯수
#define POINTNUM 1024

//몬스터설정
#define MONSTERNUM 5			//몬스터 마리수
#define WAYPOINTNUM 5		//몬스터 웨이포인트 갯수


//기본세팅
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);         //CALLBACK 매크로
HINSTANCE g_hInst;                                                                               //인스턴트 핸들
LPCTSTR lpszClass = TEXT("서동주");                                                     //제목 표시줄의 내용

//윈도우 생성 및 메세지 처리 루프
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)                                                          // 메인함수
{
	HWND hWnd;              //윈도우 핸들 선언
	MSG Message;            //메세지 구조체 변수 선언
	WNDCLASS WndClass;      // Windows Class 구조체 변수 선언
	g_hInst = hInstance;    //hinstance 값을 외부에서도 사용 할수 있도록 전역변수에 값을 저장

	WndClass.cbClsExtra = 0;                                     //예약 영역
	WndClass.cbWndExtra = 0;                                     //예약 영역
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //윈도우의 배경 색상
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);              //윈도우의 마우스포인터 모양
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);            //윈도우의 커서 모양
	WndClass.hInstance = hInstance;                              //윈도우 클래스를 등록하는 프로그램 번호
	WndClass.lpfnWndProc = WndProc;                              //윈도우 메세지 처리 함수 지정
	WndClass.lpszClassName = lpszClass;                          //윈도우 클래스의 이름 지정
	WndClass.lpszMenuName = NULL;                                //이 프로그램의 메뉴 지정
	WndClass.style = CS_HREDRAW | CS_VREDRAW;                    //윈도우의 스타일을 정의
	RegisterClass(&WndClass);                                    //WNDCLASS 구조체의 번지를 전달
																 // 윈도우를 생성


	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WNDSIZE_WIDTH, WNDSIZE_HEIGHT, //프로그램 창 크기 (500,300,300,500 과 같이 임의의 숫자로 창 크기를 변경가능)
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	//메세지 루프를 실행
	while (GetMessage(&Message, NULL, 0, 0)) {  //Queue에 있는 메세지를 읽어들인다
		TranslateMessage(&Message);             //키보드 입력 메세지를 가공하여 프로그램에서 쉽게 사용할 수 있다.
		DispatchMessage(&Message);              //메세지를 윈도우의 메세지 처리 함수 WndProc로 전달
	}

	return (int)Message.wParam;                 //탈출 코드 프로그램 종료
}


//문제1 데카르트 좌표계를 그리는 함수
void Cartesian(HDC _hdc, POINT* _pos1,POINT* _pos2,float _centerx,float _centery)
{
	//x축 y축 그리기
	for (int i = 0; i < LINESIZE; i++)
	{
		_pos1[i].x = _centerx - LINESIZE / 2 + i;
		_pos1[i].y = _centery;

		_pos2[i].x = _centerx;
		_pos2[i].y = _centery - LINESIZE / 2 + i;
	}
	for (int i = 0; i < LINESIZE; i++)
	{
		SetPixel(_hdc, _pos1[i].x, _pos1[i].y, RGB(0, 0, 0));
		SetPixel(_hdc, _pos2[i].x, _pos2[i].y, RGB(0, 0, 0));
	}
}


//좌표 변환
My_Vector ConvertingPos(My_Vector vec1, float centerx, float centery)
{
	//데카르트 좌표계로 변환 0,0이 중점
	float x = vec1.Get_X() + centerx;
	vec1.Set_X(x);
	float y = centery - vec1.Get_Y();
	vec1.Set_Y(y);

	return vec1;
}
//변환 함수
Matrix1x4 TransVec(My_Vector data1) //백터->행렬
{
	Matrix1x4 temp;

	temp.m[0][0] = data1.Get_X();
	temp.m[0][1] = data1.Get_Y();
	temp.m[0][2] = 1.0f;
	temp.m[0][3] = 1.0f;

	return temp;
}
My_Vector TransMat(Matrix1x4 data1) //행렬->백터
{
	My_Vector temp;

	temp.Set_X(data1.m[0][0]);
	temp.Set_Y(data1.m[0][1]);

	return temp;
}

//Main
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT clientRect;

	//기본 축
	static POINT Axis_x[LINESIZE];//x축
	static POINT Axis_y[LINESIZE];//y축

	//움직일 좌표와 좌표저장
	static POINT MovePos;
	static POINT SavePos;

	static My_Vector Triangle[3];			//Vector형 삼각형 꼭지점 좌표 저장
	static float GraphX[POINTNUM];		//그리기용 X좌표
	static float GraphY[POINTNUM];		//그리기용 Y좌표

	//마우스 좌표
	static float Mousex = -300;
	static float Mousey = -300;

	//키보드 좌표
	static float KeyX = 0;
	static float KeyY = 0;
	static float KeyZ = 0;
	static bool IsMove = false; //움직이고있나?

	//화면 센터x,y좌표
	static float centerx;
	static float centery;

	//시간
	static int TimeCount = 0;

	//좌표 저장 행렬(꼭지점)
	static Matrix1x4 TriangleMatrix[3];

	//클릭확인
	static bool isClick = false;

	//게임컨트롤러 생성
	static Vertex _game;


	switch (iMessage)
	{
	case WM_CREATE://윈도우 시작
	{
		SetTimer(hWnd, 2, 200, NULL);	//몬스터 이동을 위한 타이머
		//창의 크기와 중심 구하기
		GetClientRect(hWnd, &clientRect);
		centerx = clientRect.right / 2;
		centery = clientRect.bottom / 2;

		isClick = false;//클릭안함

		//vector형 좌표 설정 (꼭지점 3개)
		Triangle[0].Set_X(0.0f); Triangle[0].Set_Y(0.0f);
		Triangle[1].Set_X(-60.0f); Triangle[1].Set_Y(70.0f);
		Triangle[2].Set_X(60.0f); Triangle[2].Set_Y(70.0f);
		//데카르트 좌표계로 변환
		for (int i = 0; i < 3; i++)
		{
			Triangle[i] = ConvertingPos(Triangle[i], centerx, centery);
		}
		//Vector로 된 좌표를 Matrix로 저장
		for (int i = 0; i < 3; i++)
		{
			TriangleMatrix[i] = TransVec(Triangle[i]);
		}

		//플레이어 설정
		_game.Make_Player(TriangleMatrix);
		//몬스터 설정
		_game.Make_Monster(5, clientRect, WAYPOINTNUM);


		return 0;
	}
	case WM_PAINT:
	{

		//더블 버퍼링
		static HDC hdc, mdc;
		static HBITMAP BackBit, OldBackBit;
		static RECT BufRect;
		mdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &BufRect);
		hdc = CreateCompatibleDC(mdc);
		BackBit = CreateCompatibleBitmap(mdc, BufRect.right, BufRect.bottom);
		OldBackBit = (HBITMAP)SelectObject(hdc, BackBit);
		PatBlt(hdc, 0, 0, BufRect.right, BufRect.bottom, WHITENESS);


		Cartesian(hdc, Axis_x, Axis_y, centerx, centery);

		//상황에 따른 플레이어 그리기 (충돌 여부 전달)
		//키보드로 인한 이동
		if (IsMove)
		{
			_game._player[0].Move(KeyX, KeyY, KeyZ);
			_game._player[1].CopyPlayer(_game._player[0]);
			KeyX = 0; KeyY = 0;
			IsMove = false;
		}
		//클릭을 해서 이동중이지 않으면 실행
		if (isClick == false)
		{
			_game.Draw_Player(0, hdc, GraphX, GraphY);
		}
		else//클릭으로 인한 이동
		{
			_game._player[1].Move(MovePos.x, MovePos.y, 0);
			_game.Draw_Player(1, hdc, GraphX, GraphY);
		}

		_game.Draw_Monster(hdc, GraphX, GraphY);


		GetClientRect(hWnd, &BufRect);
		BitBlt(mdc, 0, 0, BufRect.right, BufRect.bottom, hdc, 0, 0, SRCCOPY);
		SelectObject(hdc, OldBackBit);
		DeleteObject(BackBit);
		DeleteDC(hdc);
		EndPaint(hWnd, &ps);

		break;
	}
	case WM_LBUTTONDOWN://마우스 좌클릭
	{
		if (!isClick)
		{
			//마우스 좌표 입력
			Mousex = LOWORD(lParam);
			Mousey = HIWORD(lParam);

			//관련 변수 초기화
			MovePos.x = 0;
			MovePos.y = 0;
			TimeCount = 0;

			//마우스와 기존좌표의 거리 계산 및 입력
			_game._player[0].DistancePos(Mousex, Mousey);

			//이동 완료될 좌표 저장
			SavePos.x = _game._player[0].GetMovePos().Get_X();
			SavePos.y = _game._player[0].GetMovePos().Get_Y();
			//클릭알림
			isClick = true;

			//타이머 및 
			SetTimer(hWnd, 1, 100, NULL);
		}
		else
		{
			//클릭해서 이동중에 또 눌렸을 경우
			//관련 변수 초기화
			TimeCount = 0;
			MovePos.x = 0;
			MovePos.y = 0;
			SavePos.x = 0;
			SavePos.y = 0;

			//이동중이던 copy된 플레이어를 player정보로 저장
			_game._player[0].CopyPlayer(_game._player[1]);
			Mousex = LOWORD(lParam);
			Mousey = HIWORD(lParam);

			//좌표 새로받아서 그쪽으로 이동
			_game._player[0].DistancePos(Mousex, Mousey);
			SavePos.x = _game._player[0].GetMovePos().Get_X();
			SavePos.y = _game._player[0].GetMovePos().Get_Y();
			InvalidateRect(hWnd, NULL, true);
		}
		return 0;
	}
	case WM_TIMER://타이머
	{
		switch (wParam)
		{
		case 1://클릭이동 타이머
			TimeCount++;
			MovePos.x = SavePos.x / MOVENUM;
			MovePos.y = SavePos.y / MOVENUM;
			if (TimeCount == MOVENUM + 1)
			{
				MovePos.x = 0;
				MovePos.y = 0;
				TimeCount = 0;
				isClick = false;
				_game._player[0].Move(SavePos.x, SavePos.y, 0);
				_game._player[1].CopyPlayer(_game._player[0]);
				KillTimer(hWnd, 1);
			}
			InvalidateRect(hWnd, NULL, true);
			break;
		case 2://몬스터 타이머

			_game.Management_Monst();
			if (!isClick)
			{
				InvalidateRect(hWnd, NULL, true);
			}

			break;
		default:
			break;
		}


		return 0;
	}
	case WM_KEYDOWN://키보드입력
	{
		switch (wParam)
		{
		case 'W':
			KeyY = -MOVESPEED;
			IsMove = true;
			break;
		case 'S':
			KeyY = MOVESPEED;
			IsMove = true;
			break;
		case 'A':
			KeyX = -MOVESPEED;
			IsMove = true;
			break;
		case 'D':
			KeyX = MOVESPEED;
			IsMove = true;
			break;
			//스케일, 회전 부분
		case 'E': //크기증가
		case 'Q': //크기감소
		case '1': //x축회전
		case '2'://y축회전
		case '3'://z축회전
		case VK_F1://x축 역회전
		case VK_F2://y축 역회전
		case VK_F3://z축 역회전
			_game._player[0].Mresult(wParam);
			_game._player[1].CopyPlayer(_game._player[0]);
			break;
		}
		InvalidateRect(hWnd, NULL, true);
		break;
	}
	case WM_DESTROY: //종료
	{
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		PostQuitMessage(0);
		return 0;
	}
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
