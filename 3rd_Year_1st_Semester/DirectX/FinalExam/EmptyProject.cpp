//--------------------------------------------------------------------------------------
// File: EmptyProject.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "resource.h"
#include "FinalExam_SeoDongJu_Cube.h"
#include "FinalExam_SeoDongJu_Camera.h"

//행성 갯수
#define PLANETNUM 3

//행성 구조체
struct CHARACTER
{
    D3DXVECTOR3 TranslationValue;
    LPD3DXMESH StaticMesh;
};

//큐브
CCube m_Cube;
//카메라
CCamera m_Camera;
//font 관련
LPD3DXFONT m_pTitle;
LPD3DXFONT m_pSubTitle;
//핸들
HWND m_hWnd;

//사용 변수들
D3DXMATRIX m_matProj;
CHARACTER m_planet[PLANETNUM];
D3DXMATRIX m_matScale[PLANETNUM];
D3DXMATRIX m_matRotation[PLANETNUM];
D3DXMATRIX m_matTranslation[PLANETNUM];
D3DXMATRIX m_matOrbital[PLANETNUM];

//버퍼
LPDIRECT3DVERTEXBUFFER9 m_pVB;

//--------------------------------------------------------------------------------------
// Rejects any D3D9 devices that aren't acceptable to the app by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D9DeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
                                      bool bWindowed, void* pUserContext )
{
    // Typically want to skip back buffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                                         AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
                                         D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    return true;
}

//--------------------------------------------------------------------------------------
// Create any D3D9 resources that will live through a device reset (D3DPOOL_MANAGED)
// and aren't tied to the back buffer size
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext )
{
    RECT rect;
    D3DVIEWPORT9 viewport;
    GetClientRect(m_hWnd, &rect);

    viewport.X = 0;
    viewport.Y = 0;
    viewport.Width = rect.right - rect.left;
    viewport.Height = rect.bottom - rect.top;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;
    D3DXVECTOR3 eye(15.0f, 10.0f, -10.0f);
    D3DXVECTOR3 lookat(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

    m_Camera.SetView(&eye, &lookat, &up);

    pd3dDevice->SetTransform(D3DTS_VIEW, m_Camera.GetViewMatrix());

    D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
    pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);

    pd3dDevice->SetViewport(&viewport);
    //문제 1
    D3DXCreateFont(pd3dDevice, 30, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"고딕체", &m_pTitle);

    D3DXCreateFont(pd3dDevice, 15, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"고딕체", &m_pSubTitle);

    //문제2
    m_Cube.OnInit(pd3dDevice);

    //문제3
    for (int i = 0; i < PLANETNUM; i++)
    {
        D3DXCreateSphere(pd3dDevice, 1.0f, 20, 20, &m_planet[i].StaticMesh, NULL);
    }

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT) 
// or that are tied to the back buffer size 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                    void* pUserContext )
{
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
}

void DrawAxis(IDirect3DDevice9* pd3dDevice)
{
    int AxisSize = 50.0f;
    CUSTOMVERTEX vertices[] =
    {
        {-AxisSize,	0.0f,	0.0f,	D3DCOLOR_RGBA(255,0,0,255) },
        {AxisSize,	0.0f,	0.0f,	D3DCOLOR_RGBA(255,0,0,255) },

        {0.0f,	-AxisSize,	0.0f, D3DCOLOR_RGBA(0,255,0,255) },
        {0.0f,	AxisSize,	0.0f, D3DCOLOR_RGBA(0,255,0,255) },

        {0.0f,	0.0f,	-AxisSize, D3DCOLOR_RGBA(0,0,255,255) },
        {0.0f,	0.0f,	AxisSize, D3DCOLOR_RGBA(0,0,255,255) },

    };

    pd3dDevice->CreateVertexBuffer(sizeof(vertices), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &m_pVB, NULL);

    VOID* pVertices;
    m_pVB->Lock(0, sizeof(vertices), &pVertices, 0);
    memcpy(pVertices, vertices, sizeof(vertices));
    m_pVB->Unlock();


    D3DXMATRIX matWorld;
    D3DXMatrixIdentity(&matWorld);
    pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

    pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(CUSTOMVERTEX));
    pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
    pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, 3);

    pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;
    D3DXMATRIX main_matTranslation;
    D3DXMatrixIdentity(&main_matTranslation);

    D3DXMATRIX matTranslation;
    D3DXMatrixIdentity(&matTranslation);
    
    char string[128];
    RECT rect;

    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 45, 50, 170 ), 1.0f, 0 ) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        DrawAxis(pd3dDevice);

        //문제1
        SetRect(&rect, 220, 50, 0, 0);
        m_pTitle->DrawText(NULL, L"aaaa", -1, &rect, DT_NOCLIP, D3DXCOLOR(0.5f, 1.0f, 0.0f, 1.0f));
        SetRect(&rect, 260, 80, 0, 0);
        m_pSubTitle->DrawText(NULL, L"bbbbb", -1, &rect, DT_NOCLIP, D3DXCOLOR(0.0f, 1.0f, 0.5f, 1.0f));

        //문제2
        m_Cube.OnRender();

        //문제3
        pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
        pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
        /* planet[0] draw */ //태양
        D3DXMatrixScaling(&m_matScale[0], 8.0f, 8.0f, 8.0f);
        D3DXMatrixRotationY(&m_matRotation[0], timeGetTime() / 500.0f);
        D3DXMatrixTranslation(&m_matTranslation[0], 0, 0, 0);

        matTranslation = m_matScale[0] * m_matRotation[0] * m_matTranslation[0];
        main_matTranslation = m_matRotation[0] * m_matTranslation[0];

        pd3dDevice->SetTransform(D3DTS_WORLD, &main_matTranslation);
        m_planet[0].StaticMesh->DrawSubset(0);


        /* planet[2] draw */ //금성
        D3DXMatrixScaling(&m_matScale[1], 0.25f, 0.25f, 0.25f);
        D3DXMatrixRotationY(&m_matRotation[1], timeGetTime() / 500.0f);
        D3DXMatrixTranslation(&m_matTranslation[1], -1.2f, 0.0f, 1.0f);
        D3DXMatrixRotationY(&m_matOrbital[1], timeGetTime() / 400.0f);

        matTranslation = m_matScale[1] * m_matRotation[1] * m_matTranslation[1] * m_matOrbital[1] * main_matTranslation;

        pd3dDevice->SetTransform(D3DTS_WORLD, &matTranslation);
        m_planet[1].StaticMesh->DrawSubset(0);

        /* planet[3] draw */ //지구
        D3DXMatrixScaling(&m_matScale[2], 0.25f, 0.25f, 0.25f);
        D3DXMatrixRotationY(&m_matRotation[2], timeGetTime() / 500.0f);
        D3DXMatrixTranslation(&m_matTranslation[2], 2.0f, 0.0f, 1.0f);
        D3DXMatrixRotationY(&m_matOrbital[2], timeGetTime() / 1000.0f);

        matTranslation = m_matScale[2] * m_matRotation[2] * m_matTranslation[2] * m_matOrbital[2] * main_matTranslation;

        pd3dDevice->SetTransform(D3DTS_WORLD, &matTranslation);
        m_planet[2].StaticMesh->DrawSubset(0);

        V( pd3dDevice->EndScene() );
    }
}


//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
    m_hWnd = hWnd;
    return 0;
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9ResetDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9LostDevice( void* pUserContext )
{
    SAFE_RELEASE(m_pVB);
    SAFE_RELEASE(m_pTitle);
    SAFE_RELEASE(m_pSubTitle);
    m_Cube.OnRelease();
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9CreateDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9DestroyDevice( void* pUserContext )
{
    SAFE_RELEASE(m_pVB);
    SAFE_RELEASE(m_pTitle);
    SAFE_RELEASE(m_pSubTitle);
   
    m_Cube.OnRelease();
}

//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // Set the callback functions
    DXUTSetCallbackD3D9DeviceAcceptable( IsD3D9DeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnD3D9ResetDevice );
    DXUTSetCallbackD3D9FrameRender( OnD3D9FrameRender );
    DXUTSetCallbackD3D9DeviceLost( OnD3D9LostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnD3D9DestroyDevice );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackFrameMove( OnFrameMove );

    // TODO: Perform any application-level initialization here

    // Initialize DXUT and create the desired Win32 window and Direct3D device for the application
    DXUTInit( true, true ); // Parse the command line and show msgboxes
    DXUTSetHotkeyHandling( true, true, true );  // handle the default hotkeys
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"ccccccc" );
    DXUTCreateDevice( true, 640, 480 );

    // Start the render loop
    DXUTMainLoop();

    // TODO: Perform any application-level cleanup here

    return DXUTGetExitCode();
}


