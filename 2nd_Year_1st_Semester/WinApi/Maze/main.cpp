#include <windows.h> //윈도우 헤더 파일
#include "resource.h"

#define FIGURESIZE 30 //도형의 사이즈
#define BOARDSIZE 10 //판의 크기
#define BOARDCORRECTION 60 //판 크기와 위치 보정
static int GameBoard[3][BOARDSIZE][BOARDSIZE]= //게임판
{
	{//쉬움 [0][10][10]
		{1,1,1,1,1,1,1,1,1,1},
		{0,0,1,0,1,0,1,0,1,1},
		{1,0,1,0,1,0,1,0,0,1},
		{1,0,1,0,1,0,1,1,0,1},
		{1,0,1,0,1,0,1,0,0,1},
		{1,0,1,0,1,0,1,0,1,1},
		{1,0,1,0,1,0,1,0,0,1},
		{1,0,1,0,1,0,1,1,0,1},
		{1,0,0,0,0,0,0,0,0,2},
		{1,1,1,1,1,1,1,1,1,1}
	},
	{//보통 [1][10][10]
		{1,1,1,1,1,1,1,1,1,1},
		{0,0,0,0,0,0,0,0,0,1},
		{1,1,1,0,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,1,0,1},
		{1,0,1,1,1,1,1,1,0,1},
		{1,0,1,0,0,0,1,1,0,1},
		{1,0,0,0,1,0,0,1,0,1},
		{1,0,1,1,1,1,0,1,0,1},
		{1,0,0,0,0,1,0,0,0,2},
		{1,1,1,1,1,1,1,1,1,1}
	},
	{//어려움 [2][10][10]
		{1,1,1,1,1,1,1,1,1,1},
		{0,0,0,1,0,1,0,0,0,1},
		{1,0,1,0,0,0,0,1,0,1},
		{1,0,0,0,1,1,0,1,1,1},
		{1,0,1,0,1,0,1,0,0,1},
		{1,1,0,0,1,0,0,0,1,1},
		{1,0,0,0,1,0,1,0,0,1},
		{1,0,1,0,1,0,0,1,0,1},
		{1,0,1,0,0,0,1,0,0,2},
		{1,1,1,1,1,1,1,1,1,1}
	}
};



LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); //CALLBACK 매크로
HINSTANCE g_hInst; //인스턴트 핸들
HWND hWndMain;
LPCTSTR lpszClass = TEXT(""); //제목 표시줄 내용

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) //메인함수
{
	HWND hWnd;				//윈도우 핸들 선언
	MSG Message;			//메세지 구조체 변수 선언
	WNDCLASS WndClass;		//Windows Class 구조체 변수 선언
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	//윈도우 배경 색상
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);					//마우스 포인터 모양
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);				//커서 모양
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);			//프로그램 창 크기 조정
	ShowWindow(hWnd, nCmdShow);

	//메세지 루프 실행
	while (GetMessage(&Message, NULL, 0, 0)) {
		if ((Message.message == WM_KEYDOWN || Message.message == WM_KEYUP) && Message.hwnd != hWnd)
		{
			PostMessage(hWnd, Message.message, Message.wParam, Message.lParam);
		}
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;			//탈출 코드 프로그램 종료
}

bool Check(int x, int y,int boardtype) //앞에 벽이 있는지 확인하는 함수
{
	if (GameBoard[boardtype][y][x] == 1)//벽
	{
		return true;
	}
	return false;
}
bool GameClear(int x, int y, int boardtype) //게임 목표를 달성했는지 확인하는 함수
{
	if (GameBoard[boardtype][y][x] == 2)//도달지점
	{
		return true;
	}
	return false;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{

	static int BoardType;
	static int x_pos; //x좌표
	static int y_pos;//y좌표

	switch (iMessage) {
	case WM_CREATE: //초기값설정
	{
		BoardType = 0;
		x_pos = 0;
		y_pos = 1;
		break;
	}

	case WM_COMMAND: //메뉴 설정
		switch (LOWORD(wParam)) 
		{
		case ID_LV1: //쉬움
			BoardType = 0;
			x_pos = 0;
			y_pos = 1;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case ID_LV2: //보통
			BoardType = 1;
			x_pos = 0;
			y_pos = 1;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case ID_LV3://어려움
			BoardType = 2;
			x_pos = 0;
			y_pos = 1;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}


	case WM_PAINT:
	{
		HDC hdc;
		PAINTSTRUCT ps;
		HBRUSH hBrush, oldBrush;//브러쉬 생성

		hdc = BeginPaint(hWnd, &ps);
		//색설정
		hBrush = CreateSolidBrush(RGB(255, 0, 0));
		oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		for (int i = 0; i < BOARDSIZE; i++) //게임판 그려주기
		{
			for (int j = 0; j < BOARDSIZE; j++)
			{
				if (GameBoard[BoardType][i][j] == 1)//벽 생성
				{
					Rectangle(hdc, j * BOARDCORRECTION - FIGURESIZE+ BOARDCORRECTION, i * BOARDCORRECTION - FIGURESIZE + BOARDCORRECTION, j * BOARDCORRECTION + FIGURESIZE + BOARDCORRECTION, i * BOARDCORRECTION + FIGURESIZE + BOARDCORRECTION);
				}
				if (GameBoard[BoardType][i][j] == 2)//클리어 위치
				{
					hBrush = CreateSolidBrush(RGB(0, 0, 255));
					oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
					Rectangle(hdc, j * BOARDCORRECTION - FIGURESIZE + BOARDCORRECTION, i * BOARDCORRECTION - FIGURESIZE + BOARDCORRECTION, j * BOARDCORRECTION + FIGURESIZE + BOARDCORRECTION, i * BOARDCORRECTION + FIGURESIZE + BOARDCORRECTION);
					SelectObject(hdc, oldBrush);// 이전의 브러쉬로 돌아감
					DeleteObject(hBrush);
				}
			}
		}
		SelectObject(hdc, oldBrush);// 이전의 브러쉬로 돌아감
		DeleteObject(hBrush);

		hBrush = CreateSolidBrush(RGB(0, 255, 0));
		oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		//이동하는 원 그려주기
		Ellipse(hdc, x_pos * BOARDCORRECTION - FIGURESIZE + BOARDCORRECTION, y_pos * BOARDCORRECTION - FIGURESIZE + BOARDCORRECTION, x_pos * BOARDCORRECTION + FIGURESIZE + BOARDCORRECTION, y_pos * BOARDCORRECTION + FIGURESIZE + BOARDCORRECTION);
		SelectObject(hdc, oldBrush);// 이전의 브러쉬로 돌아감
		DeleteObject(hBrush);
		// 게임 정보
		hBrush = CreateSolidBrush(RGB(0, 0, 255));
		oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		TextOut(hdc, 750, 150, TEXT("이 게임의 목표는 초록색원을 목표지점까지"), 40);
		TextOut(hdc, 750, 180, TEXT("이동하는 미로 게임입니다."), 25);
		TextOut(hdc, 820, 230, TEXT("<< 파란색 사각형이 목표지점입니다."), 34);
		Rectangle(hdc, 12 * BOARDCORRECTION - FIGURESIZE + BOARDCORRECTION, 3 * BOARDCORRECTION - FIGURESIZE + BOARDCORRECTION, 12 * BOARDCORRECTION + FIGURESIZE + BOARDCORRECTION, 3 * BOARDCORRECTION + FIGURESIZE + BOARDCORRECTION);
		SelectObject(hdc, oldBrush);// 이전의 브러쉬로 돌아감
		DeleteObject(hBrush);
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_KEYDOWN: //키보드 컨트롤
	{
		switch (wParam)
		{
		case VK_LEFT:
			if(Check(x_pos-1,y_pos,BoardType)!=true) //벽이 있는지 확인후 이동
			x_pos--;
			break;
		case VK_RIGHT:
			if (Check(x_pos+1, y_pos, BoardType) != true)//벽이 있는지 확인후 이동
			x_pos++;
			break;
		case VK_UP:
			if (Check(x_pos, y_pos-1, BoardType) != true)//벽이 있는지 확인후 이동
			y_pos--;
			break;
		case VK_DOWN:
			if (Check(x_pos, y_pos+1, BoardType) != true)//벽이 있는지 확인후 이동
			y_pos++;
			
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		if (GameClear(x_pos, y_pos, BoardType))//이동후에 목표지점에 도달햇는지 체크
			MessageBox(hWnd, TEXT("목표 지점에 도달했습니다."), TEXT("클리어!"), MB_OK);
		break;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
