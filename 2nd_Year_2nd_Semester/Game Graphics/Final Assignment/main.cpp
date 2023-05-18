#include <windows.h>			//������ ��� ����
#include "My_Vertex.h"

//ȭ��ũ��
#define WNDSIZE_WIDTH 1980
#define WNDSIZE_HEIGHT 1080
//���� ������ Ƚ��
#define MOVENUM 5
//Ű���� �̵�������
#define MOVESPEED 10
//�������
#define LINESIZE 2048
//���� ����
#define POINTNUM 1024

//���ͼ���
#define MONSTERNUM 5			//���� ������
#define WAYPOINTNUM 5		//���� ��������Ʈ ����


//�⺻����
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);         //CALLBACK ��ũ��
HINSTANCE g_hInst;                                                                               //�ν���Ʈ �ڵ�
LPCTSTR lpszClass = TEXT("������");                                                     //���� ǥ������ ����

//������ ���� �� �޼��� ó�� ����
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)                                                          // �����Լ�
{
	HWND hWnd;              //������ �ڵ� ����
	MSG Message;            //�޼��� ����ü ���� ����
	WNDCLASS WndClass;      // Windows Class ����ü ���� ����
	g_hInst = hInstance;    //hinstance ���� �ܺο����� ��� �Ҽ� �ֵ��� ���������� ���� ����

	WndClass.cbClsExtra = 0;                                     //���� ����
	WndClass.cbWndExtra = 0;                                     //���� ����
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //�������� ��� ����
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);              //�������� ���콺������ ���
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);            //�������� Ŀ�� ���
	WndClass.hInstance = hInstance;                              //������ Ŭ������ ����ϴ� ���α׷� ��ȣ
	WndClass.lpfnWndProc = WndProc;                              //������ �޼��� ó�� �Լ� ����
	WndClass.lpszClassName = lpszClass;                          //������ Ŭ������ �̸� ����
	WndClass.lpszMenuName = NULL;                                //�� ���α׷��� �޴� ����
	WndClass.style = CS_HREDRAW | CS_VREDRAW;                    //�������� ��Ÿ���� ����
	RegisterClass(&WndClass);                                    //WNDCLASS ����ü�� ������ ����
																 // �����츦 ����


	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WNDSIZE_WIDTH, WNDSIZE_HEIGHT, //���α׷� â ũ�� (500,300,300,500 �� ���� ������ ���ڷ� â ũ�⸦ ���氡��)
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	//�޼��� ������ ����
	while (GetMessage(&Message, NULL, 0, 0)) {  //Queue�� �ִ� �޼����� �о���δ�
		TranslateMessage(&Message);             //Ű���� �Է� �޼����� �����Ͽ� ���α׷����� ���� ����� �� �ִ�.
		DispatchMessage(&Message);              //�޼����� �������� �޼��� ó�� �Լ� WndProc�� ����
	}

	return (int)Message.wParam;                 //Ż�� �ڵ� ���α׷� ����
}


//����1 ��ī��Ʈ ��ǥ�踦 �׸��� �Լ�
void Cartesian(HDC _hdc, POINT* _pos1,POINT* _pos2,float _centerx,float _centery)
{
	//x�� y�� �׸���
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


//��ǥ ��ȯ
My_Vector ConvertingPos(My_Vector vec1, float centerx, float centery)
{
	//��ī��Ʈ ��ǥ��� ��ȯ 0,0�� ����
	float x = vec1.Get_X() + centerx;
	vec1.Set_X(x);
	float y = centery - vec1.Get_Y();
	vec1.Set_Y(y);

	return vec1;
}
//��ȯ �Լ�
Matrix1x4 TransVec(My_Vector data1) //����->���
{
	Matrix1x4 temp;

	temp.m[0][0] = data1.Get_X();
	temp.m[0][1] = data1.Get_Y();
	temp.m[0][2] = 1.0f;
	temp.m[0][3] = 1.0f;

	return temp;
}
My_Vector TransMat(Matrix1x4 data1) //���->����
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

	//�⺻ ��
	static POINT Axis_x[LINESIZE];//x��
	static POINT Axis_y[LINESIZE];//y��

	//������ ��ǥ�� ��ǥ����
	static POINT MovePos;
	static POINT SavePos;

	static My_Vector Triangle[3];			//Vector�� �ﰢ�� ������ ��ǥ ����
	static float GraphX[POINTNUM];		//�׸���� X��ǥ
	static float GraphY[POINTNUM];		//�׸���� Y��ǥ

	//���콺 ��ǥ
	static float Mousex = -300;
	static float Mousey = -300;

	//Ű���� ��ǥ
	static float KeyX = 0;
	static float KeyY = 0;
	static float KeyZ = 0;
	static bool IsMove = false; //�����̰��ֳ�?

	//ȭ�� ����x,y��ǥ
	static float centerx;
	static float centery;

	//�ð�
	static int TimeCount = 0;

	//��ǥ ���� ���(������)
	static Matrix1x4 TriangleMatrix[3];

	//Ŭ��Ȯ��
	static bool isClick = false;

	//������Ʈ�ѷ� ����
	static Vertex _game;


	switch (iMessage)
	{
	case WM_CREATE://������ ����
	{
		SetTimer(hWnd, 2, 200, NULL);	//���� �̵��� ���� Ÿ�̸�
		//â�� ũ��� �߽� ���ϱ�
		GetClientRect(hWnd, &clientRect);
		centerx = clientRect.right / 2;
		centery = clientRect.bottom / 2;

		isClick = false;//Ŭ������

		//vector�� ��ǥ ���� (������ 3��)
		Triangle[0].Set_X(0.0f); Triangle[0].Set_Y(0.0f);
		Triangle[1].Set_X(-60.0f); Triangle[1].Set_Y(70.0f);
		Triangle[2].Set_X(60.0f); Triangle[2].Set_Y(70.0f);
		//��ī��Ʈ ��ǥ��� ��ȯ
		for (int i = 0; i < 3; i++)
		{
			Triangle[i] = ConvertingPos(Triangle[i], centerx, centery);
		}
		//Vector�� �� ��ǥ�� Matrix�� ����
		for (int i = 0; i < 3; i++)
		{
			TriangleMatrix[i] = TransVec(Triangle[i]);
		}

		//�÷��̾� ����
		_game.Make_Player(TriangleMatrix);
		//���� ����
		_game.Make_Monster(5, clientRect, WAYPOINTNUM);


		return 0;
	}
	case WM_PAINT:
	{

		//���� ���۸�
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

		//��Ȳ�� ���� �÷��̾� �׸��� (�浹 ���� ����)
		//Ű����� ���� �̵�
		if (IsMove)
		{
			_game._player[0].Move(KeyX, KeyY, KeyZ);
			_game._player[1].CopyPlayer(_game._player[0]);
			KeyX = 0; KeyY = 0;
			IsMove = false;
		}
		//Ŭ���� �ؼ� �̵������� ������ ����
		if (isClick == false)
		{
			_game.Draw_Player(0, hdc, GraphX, GraphY);
		}
		else//Ŭ������ ���� �̵�
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
	case WM_LBUTTONDOWN://���콺 ��Ŭ��
	{
		if (!isClick)
		{
			//���콺 ��ǥ �Է�
			Mousex = LOWORD(lParam);
			Mousey = HIWORD(lParam);

			//���� ���� �ʱ�ȭ
			MovePos.x = 0;
			MovePos.y = 0;
			TimeCount = 0;

			//���콺�� ������ǥ�� �Ÿ� ��� �� �Է�
			_game._player[0].DistancePos(Mousex, Mousey);

			//�̵� �Ϸ�� ��ǥ ����
			SavePos.x = _game._player[0].GetMovePos().Get_X();
			SavePos.y = _game._player[0].GetMovePos().Get_Y();
			//Ŭ���˸�
			isClick = true;

			//Ÿ�̸� �� 
			SetTimer(hWnd, 1, 100, NULL);
		}
		else
		{
			//Ŭ���ؼ� �̵��߿� �� ������ ���
			//���� ���� �ʱ�ȭ
			TimeCount = 0;
			MovePos.x = 0;
			MovePos.y = 0;
			SavePos.x = 0;
			SavePos.y = 0;

			//�̵����̴� copy�� �÷��̾ player������ ����
			_game._player[0].CopyPlayer(_game._player[1]);
			Mousex = LOWORD(lParam);
			Mousey = HIWORD(lParam);

			//��ǥ ���ι޾Ƽ� �������� �̵�
			_game._player[0].DistancePos(Mousex, Mousey);
			SavePos.x = _game._player[0].GetMovePos().Get_X();
			SavePos.y = _game._player[0].GetMovePos().Get_Y();
			InvalidateRect(hWnd, NULL, true);
		}
		return 0;
	}
	case WM_TIMER://Ÿ�̸�
	{
		switch (wParam)
		{
		case 1://Ŭ���̵� Ÿ�̸�
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
		case 2://���� Ÿ�̸�

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
	case WM_KEYDOWN://Ű�����Է�
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
			//������, ȸ�� �κ�
		case 'E': //ũ������
		case 'Q': //ũ�Ⱘ��
		case '1': //x��ȸ��
		case '2'://y��ȸ��
		case '3'://z��ȸ��
		case VK_F1://x�� ��ȸ��
		case VK_F2://y�� ��ȸ��
		case VK_F3://z�� ��ȸ��
			_game._player[0].Mresult(wParam);
			_game._player[1].CopyPlayer(_game._player[0]);
			break;
		}
		InvalidateRect(hWnd, NULL, true);
		break;
	}
	case WM_DESTROY: //����
	{
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		PostQuitMessage(0);
		return 0;
	}
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
