//2020.03.31
#include <windows.h>                                                                             //윈도우 헤더 파일

#define WNDSIZE_WIDTH 800
#define WNDSIZE_HEIGHT 600

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);                                            //CALLBACK 매크로
HINSTANCE g_hInst;                                                                               //인스턴트 핸들
LPCTSTR lpszClass = TEXT("Omok");                                                     //제목 표시줄의 내용

//새로 알게된 사실 오목판은 15x15다... 바둑판이 19x19
#define X_Omoksize 15
#define Y_Omoksize 15

#define X_Space 50
#define Y_Space 50

#define INTERVAL 25
#define INTERVAL_Half INTERVAL/2

#define XPOS(x)   (X_Space + (x) * INTERVAL) 
#define YPOS(y)   (Y_Space + (y) * INTERVAL) 

int user = 0; //순서 흑색 바둑알부터 시작

int save[INTERVAL][INTERVAL];

int W_B, W_W;//승리횟수
int end;//끝확인

void Game_End_X();
void Game_End_Y();
void Game_End_Rdown();
void Game_End_Ldown();

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)                                                          // 메인함수
{
    HWND hWnd;              //윈도우 핸들 선언
    MSG Message;            //메세지 구조체 변수 선언
    WNDCLASS WndClass;      // Windows Class 구조체 변수 선언
    g_hInst = hInstance;    //hinstance 값을 외부에서도 사용 할수 있도록 전역변수에 값을 저장

    WndClass.cbClsExtra = 0;                                     //예약 영역, 지금은 사용 x
    WndClass.cbWndExtra = 0;                                     //예약 영역
    WndClass.hbrBackground = CreateSolidBrush(RGB(150,75,0)); //윈도우의 배경 색상
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    static RECT clientRect;
    HDC hdc;
    PAINTSTRUCT ps;

    int i, l;
    static int mx, my; // 마우스좌표

    switch (iMessage) {
    case WM_CREATE:
    {
        GetClientRect(hWnd, &clientRect);

        CreateWindow(TEXT("button"), TEXT("초기화"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 500, 200, 100, 25, hWnd, (HMENU)0, g_hInst, NULL);

        return 0;
    }

    case WM_COMMAND:
    {
        int x, y;
        switch (LOWORD(wParam))
        {
        case 0://리셋

            InvalidateRect(hWnd, NULL, TRUE);

            memset(save, 0, INTERVAL* INTERVAL * sizeof(int));
            end = 0;
            break;
        }
        return 0;
    }
    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);
        //바둑판 15x15
        TextOut(hdc, 500, 100, TEXT("흑돌의 차례입니다."), 10);//첫 순서
        TCHAR str[128];
        wsprintf(str, TEXT(" 승리횟수:  흑돌 : %d VS %d: 백돌  "), W_B, W_W); // 0:0 으로시작
        TextOut(hdc, 500, 150, str, lstrlen(str));
        for (i = 0; i < X_Omoksize;i++)
        {
            MoveToEx(hdc, XPOS(i), YPOS(0), NULL);
            LineTo(hdc, XPOS(i), YPOS(Y_Omoksize-1));
        }
        for (l = 0; l < Y_Omoksize; l++)
        {
            MoveToEx(hdc, XPOS(0), YPOS(l), NULL);
            LineTo(hdc, XPOS(X_Omoksize-1), YPOS(l));
        }
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_LBUTTONDOWN: //마우스 왼 클릭시 진행
        hdc = GetDC(hWnd);
        mx = LOWORD(lParam);
        my = HIWORD(lParam);

        if (end == 1)//승리판정 확인
        {
            MessageBox(hWnd, TEXT("게임이 끝났습니다. 초기화버튼을 눌러주세요"), TEXT("끝!"), MB_OK);
        }
        else 
        {
            if (mx > (XPOS(0) - INTERVAL_Half) && my > (YPOS(0) - INTERVAL_Half) && mx < (XPOS(X_Omoksize - 1) + INTERVAL_Half) && my < (YPOS(Y_Omoksize - 1) + INTERVAL_Half))//안나가게 설정
            {
                //오목돌 보정
                int x = mx / INTERVAL;
                int y = my / INTERVAL;
                x = x * INTERVAL;
                y = y * INTERVAL;

                if (save[x/ INTERVAL][y/ INTERVAL] == 1 || save[x/ INTERVAL][y/ INTERVAL] == 6)//이미 돌이 있는 곳에 클릭했는지 확인!
                {
                    MessageBox(hWnd, TEXT("중복된 칸에 클릭하셨습니다."), TEXT("중복!"), MB_OK);
                }
                else// 돌이없으면 실행
                {
                    if (user % 2 == 0)//흑
                    {

                        HBRUSH myBrush = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
                        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, myBrush);
                        Ellipse(hdc, x - INTERVAL_Half, y - INTERVAL_Half, x + INTERVAL_Half, y + INTERVAL_Half);
                        SelectObject(hdc, oldBrush);
                        DeleteObject(myBrush);

                        TextOut(hdc, 500, 100, TEXT("백돌의 차례입니다."), 10);//차례 확인
                        TCHAR str[128];
                        user = user + 1;// 다음턴
                        save[x/ INTERVAL][y/ INTERVAL] = user % 2; //돌의 정보 저장
                        //이밑에 게임승리
                        Game_End_X(hWnd);
                        Game_End_Y(hWnd);
                        Game_End_Rdown(hWnd);
                        Game_End_Ldown(hWnd);
                    }
                    else//백
                    {
                        Ellipse(hdc, x - INTERVAL_Half, y - INTERVAL_Half, x + INTERVAL_Half, y + INTERVAL_Half);

                        TextOut(hdc, 500, 100, TEXT("흑돌의 차례입니다."), 10);//차례 확인 
                        TCHAR str[128];
                        user = user + 1;// 다음턴
                        save[x/INTERVAL][y/INTERVAL] = user % 2 + 6;//돌의 정보 저장
                        //이밑 게임승리
                        Game_End_X(hWnd);
                        Game_End_Y(hWnd);
                        Game_End_Rdown(hWnd);
                        Game_End_Ldown(hWnd);
                    }
                }
                TCHAR str[128];
                wsprintf(str, TEXT(" 승리횟수:  흑돌 : %d VS %d: 백돌  "), W_B, W_W); //승리횟수 체크
                TextOut(hdc, 500, 150, str, lstrlen(str));
            }
        }
        ReleaseDC(hWnd, hdc);
        return 0;
    case WM_DESTROY: //종료
    {
        PostQuitMessage(0);
        return 0;
    }
    }
    return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
//승리 탐색
void Game_End_X(HWND hWnd)
{
    int ex, ey,ez;
    int result = 0;
    //X축 검사
    for (ey = 0; ey < X_Omoksize; ey++)
    {
        for (ex = 0; ex < Y_Omoksize; ex++)
        {
            for (ez = 0; ez < 5; ez = ez++)
            {
                result = result + save[ex+ez][ey];
                if (result == 5)
                {
                    MessageBox(hWnd, TEXT("흑이 승리하였습니다."), TEXT("게임끝!"), MB_OK);
                    end = 1;
                    W_B++;
                    return 0;
                }
                else if (result == 30)
                {
                    MessageBox(hWnd, TEXT("백이 승리하였습니다."), TEXT("게임끝!"), MB_OK);
                    end = 1;
                    W_W++;
                    return 0;
                }
            }
            result = 0;
        }
    }
}

void Game_End_Y(HWND hWnd)
{
    int ex, ey,ez;
    int result = 0;
    //y축 검사
    for (ex = 0; ex < Y_Omoksize; ex++)
    {
        for (ey = 0; ey < X_Omoksize; ey++)
        {
            for (ez = 0; ez < 5; ez++)
            {
                result = result + save[ex][ey+ez];
                if (result == 5)
                {
                    MessageBox(hWnd, TEXT("흑이 승리하였습니다."), TEXT("게임끝!"), MB_OK);
                    end = 1;
                    W_B++;
                    return 0;
                }
                else if (result == 30)
                {
                    MessageBox(hWnd, TEXT("백이 승리하였습니다."), TEXT("게임끝!"), MB_OK);
                    end = 1;
                    W_W++;
                    return 0;
                }
            }
            result = 0;
        }
    }
}

void Game_End_Rdown(HWND hWnd)
{
    int ex, ey, ez;
    int result = 0;
    //↘대각선 검사
    for (ex = 0; ex < Y_Omoksize; ex++)
    {
        for (ey = 0; ey < X_Omoksize; ey++)
        {
            for (ez = 0; ez < 5; ez ++)
            {
                result = result + save[ex+ez][ey + ez];
                if (result == 5)
                {
                    MessageBox(hWnd, TEXT("흑이 승리하였습니다."), TEXT("게임끝!"), MB_OK);
                    end = 1;
                    W_B++;
                    return 0;
                }
                else if (result == 30)
                {
                    MessageBox(hWnd, TEXT("백이 승리하였습니다."), TEXT("게임끝!"), MB_OK);
                    end = 1;
                    W_W++;
                    return 0;
                }
            }
            result = 0;
        }
    }
}

void Game_End_Ldown(HWND hWnd)
{
    int ex, ey, ez;
    int result = 0;
    //↙대각선 검사
    for (ex = 0; ex < Y_Omoksize; ex++)
    {
        for (ey = X_Omoksize; ey >= 0; ey--)
        {
            for (ez = 0; ez < 5; ez++)
            {
                result = result + save[ex-ez][ey + ez];
                if (result == 5)
                {
                    MessageBox(hWnd, TEXT("흑이 승리하였습니다."), TEXT("게임끝!"), MB_OK);
                    end = 1;
                    W_B++;
                    return 0;
                }
                else if (result == 30)
                {
                    MessageBox(hWnd, TEXT("백이 승리하였습니다."), TEXT("게임끝!"), MB_OK);
                    end = 1;
                    W_W++;
                    return 0;
                }
            }
            result = 0;
        }
    }
}