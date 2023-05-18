//-----------------------------------------------------------------------------
// File: Lights.cpp
//
// Desc: Rendering 3D geometry is much more interesting when dynamic lighting
//       is added to the scene. To use lighting in D3D, you must create one or
//       lights, setup a material, and make sure your geometry contains surface
//       normals. Lights may have a position, a color, and be of a certain type
//       such as directional (light comes from one direction), point (light
//       comes from a specific x,y,z coordinate and radiates in all directions)
//       or spotlight. Materials describe the surface of your geometry,
//       specifically, how it gets lit (diffuse color, ambient color, etc.).
//       Surface normals are part of a vertex, and are needed for the D3D's
//       internal lighting calculations.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#include <strsafe.h>
#include <time.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 



// 1. 조명의 색깔을 바꿔 보고
// 2. 5초에 한번씩 색깔이 자동으로 바뀌도록 (랜덤하게. 레인보우)
// 3. 원기둥을 두 개 만들어서 돌려봅시다.
// 4. 두개의 원기둥에 가각 다른 텍스쳐 입히기
// 5.Vertex 를 이용해서 일일히 그림을 그리는 방법 말고, 
// DX 에서 제공해주는 기본 primitive 들이 있다. 
// DrawXXXX
// help 파일, MSDN 만 이 두개만 이용해서 찾아서 될 수 있는 한 모든것을 다 그려봅시다. 
// 조명, 회전애니메이션 이런거는 옵션으로 해봅시다. 


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device
LPDIRECT3DVERTEXBUFFER9 g_pVB_1 = NULL; // 태양
LPDIRECT3DVERTEXBUFFER9 g_pVB_2 = NULL; // 지구
LPDIRECT3DVERTEXBUFFER9 g_pVB_3 = NULL; // 금성
LPDIRECT3DVERTEXBUFFER9 g_pVB_4 = NULL; // 달
LPDIRECT3DVERTEXBUFFER9 g_pVB_5 = NULL; // 수성

#define PLANETCOUNT  5

//구조체 배열
struct _SRotation
{
    //회전 각 x,y,z
    float rotation_x = 0.0f;
    float rotation_y = 0.0f;
    float rotation_z = 0.0f;

}SRotation[PLANETCOUNT];


// A structure for our custom vertex type. We added a normal, and omitted the
// color (which is provided by the material)
struct CUSTOMVERTEX
{
    D3DXVECTOR3 position; // The 3D position for the vertex
    D3DXVECTOR3 normal;   // The surface normal for the vertex
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL)




//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
    // Create the D3D object.
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice. Since we are now
    // using more complex geometry, we will create a device with a zbuffer.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.Windowed = TRUE;                                              //창모드로 설정
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    // Create the D3DDevice
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    // Turn off culling
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // Turn on the zbuffer
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Creates the scene geometry
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{

    // 1. 버텍스 버퍼를 먼저 만들었다. 
    // 2. Lock 을 걸고 (복사 하는 부분) 바로 그 안에서 알고리즘으로 생성

    // Create the vertex buffer.
    //if( FAILED( g_pd3dDevice->CreateVertexBuffer( 50 * 2 * sizeof( CUSTOMVERTEX ),
    //                                              0, D3DFVF_CUSTOMVERTEX,
    //                                              D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
    //{
    //    return E_FAIL;
    //}

    //// Fill the vertex buffer. We are algorithmically generating a cylinder
    //// here, including the normals, which are used for lighting.
    //CUSTOMVERTEX* pVertices;
    //if( FAILED( g_pVB->Lock( 0, 0, ( void** )&pVertices, 0 ) ) )
    //    return E_FAIL;
    //for( DWORD i = 0; i < 50; i++ )
    //{
    //    FLOAT theta = ( 2 * D3DX_PI * i ) / ( 50 - 1 );
    //    pVertices[2 * i + 0].position = D3DXVECTOR3( sinf( theta ), -1.0f, cosf( theta ) );
    //    pVertices[2 * i + 0].normal = D3DXVECTOR3( sinf( theta ), 0.0f, cosf( theta ) );
    //    pVertices[2 * i + 1].position = D3DXVECTOR3( sinf( theta ), 1.0f, cosf( theta ) );
    //    pVertices[2 * i + 1].normal = D3DXVECTOR3( sinf( theta ), 0.0f, cosf( theta ) );
    //}
    //g_pVB->Unlock();

    //return S_OK;


    //1. 우리가 사용할 프로그래밍적 버택스를 만들고
    VOID* pVertices_1;
    VOID* pVertices_2;
    VOID* pVertices_3;
    VOID* pVertices_4;
    VOID* pVertices_5;


    CUSTOMVERTEX vertices_1[100];
    for (DWORD i = 0; i < 50; i++)
    {
        FLOAT theta = (2 * D3DX_PI * i) / (50 - 1);
        vertices_1[2 * i + 0].position = D3DXVECTOR3(sinf(theta), -1.0f, cosf(theta));
        vertices_1[2 * i + 0].normal = D3DXVECTOR3(sinf(theta), 0.0f, cosf(theta));
        vertices_1[2 * i + 1].position = D3DXVECTOR3(sinf(theta), 1.0f, cosf(theta));
        vertices_1[2 * i + 1].normal = D3DXVECTOR3(sinf(theta), 0.0f, cosf(theta));
    }
    CUSTOMVERTEX vertices_2[100];
    for (DWORD i = 0; i < 50; i++)
    {
        FLOAT theta = (2 * D3DX_PI * i) / (50 - 1);
        vertices_2[2 * i + 0].position = D3DXVECTOR3(sinf(theta), -1.0f, cosf(theta));
        vertices_2[2 * i + 0].normal = D3DXVECTOR3(sinf(theta), 0.0f, cosf(theta));
        vertices_2[2 * i + 1].position = D3DXVECTOR3(sinf(theta), 1.0f, cosf(theta));
        vertices_2[2 * i + 1].normal = D3DXVECTOR3(sinf(theta), 0.0f, cosf(theta));
    }
    CUSTOMVERTEX vertices_3[100];
    for (DWORD i = 0; i < 50; i++)
    {
        FLOAT theta = (2 * D3DX_PI * i) / (50 - 1);
        vertices_3[2 * i + 0].position = D3DXVECTOR3(sinf(theta), -1.0f, cosf(theta));
        vertices_3[2 * i + 0].normal = D3DXVECTOR3(sinf(theta), 0.0f, cosf(theta));
        vertices_3[2 * i + 1].position = D3DXVECTOR3(sinf(theta), 1.0f, cosf(theta));
        vertices_3[2 * i + 1].normal = D3DXVECTOR3(sinf(theta), 0.0f, cosf(theta));
    }
    CUSTOMVERTEX vertices_4[100];
    for (DWORD i = 0; i < 50; i++)
    {
        FLOAT theta = (2 * D3DX_PI * i) / (50 - 1);
        vertices_4[2 * i + 0].position = D3DXVECTOR3(sinf(theta), -1.0f, cosf(theta));
        vertices_4[2 * i + 0].normal = D3DXVECTOR3(sinf(theta), 0.0f, cosf(theta));
        vertices_4[2 * i + 1].position = D3DXVECTOR3(sinf(theta), 1.0f, cosf(theta));
        vertices_4[2 * i + 1].normal = D3DXVECTOR3(sinf(theta), 0.0f, cosf(theta));
    }
    CUSTOMVERTEX vertices_5[100];
    for (DWORD i = 0; i < 50; i++)
    {
        FLOAT theta = (2 * D3DX_PI * i) / (50 - 1);
        vertices_5[2 * i + 0].position = D3DXVECTOR3(sinf(theta), -1.0f, cosf(theta));
        vertices_5[2 * i + 0].normal = D3DXVECTOR3(sinf(theta), 0.0f, cosf(theta));
        vertices_5[2 * i + 1].position = D3DXVECTOR3(sinf(theta), 1.0f, cosf(theta));
        vertices_5[2 * i + 1].normal = D3DXVECTOR3(sinf(theta), 0.0f, cosf(theta));
    }

    //2. 여기에 맞게 버택스 버퍼를 생성하고
    if (FAILED(g_pd3dDevice->CreateVertexBuffer(50 * 2 * sizeof(CUSTOMVERTEX),
        0, D3DFVF_CUSTOMVERTEX,
        D3DPOOL_DEFAULT, &g_pVB_1, NULL)))
    {
        return E_FAIL;
    }
    if (FAILED(g_pd3dDevice->CreateVertexBuffer(50 * 2 * sizeof(CUSTOMVERTEX),
        0, D3DFVF_CUSTOMVERTEX,
        D3DPOOL_DEFAULT, &g_pVB_2, NULL)))
    {
        return E_FAIL;
    }
    if (FAILED(g_pd3dDevice->CreateVertexBuffer(50 * 2 * sizeof(CUSTOMVERTEX),
        0, D3DFVF_CUSTOMVERTEX,
        D3DPOOL_DEFAULT, &g_pVB_3, NULL)))
    {
        return E_FAIL;
    }
    if (FAILED(g_pd3dDevice->CreateVertexBuffer(50 * 2 * sizeof(CUSTOMVERTEX),
        0, D3DFVF_CUSTOMVERTEX,
        D3DPOOL_DEFAULT, &g_pVB_4, NULL)))
    {
        return E_FAIL;
    }
    if (FAILED(g_pd3dDevice->CreateVertexBuffer(50 * 2 * sizeof(CUSTOMVERTEX),
        0, D3DFVF_CUSTOMVERTEX,
        D3DPOOL_DEFAULT, &g_pVB_5, NULL)))
    {
        return E_FAIL;
    }

    //3. 복사
    if (FAILED(g_pVB_1->Lock(0, 0, (void**)&pVertices_1, 0)))
        return E_FAIL;
    memcpy(pVertices_1, vertices_1, sizeof(vertices_1));
    g_pVB_1->Unlock();

    if (FAILED(g_pVB_2->Lock(0, 0, (void**)&pVertices_2, 0)))
        return E_FAIL;
    memcpy(pVertices_2, vertices_2, sizeof(vertices_2));
    g_pVB_2->Unlock();

    if (FAILED(g_pVB_3->Lock(0, 0, (void**)&pVertices_3, 0)))
        return E_FAIL;
    memcpy(pVertices_3, vertices_3, sizeof(vertices_3));
    g_pVB_3->Unlock();

    if (FAILED(g_pVB_4->Lock(0, 0, (void**)&pVertices_4, 0)))
        return E_FAIL;
    memcpy(pVertices_4, vertices_4, sizeof(vertices_4));
    g_pVB_4->Unlock();

    if (FAILED(g_pVB_5->Lock(0, 0, (void**)&pVertices_5, 0)))
        return E_FAIL;
    memcpy(pVertices_5, vertices_5, sizeof(vertices_5));
    g_pVB_5->Unlock();


    return S_OK;

}




//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    if( g_pVB_1 != NULL )
        g_pVB_1->Release();

    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
}



//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------

//중심
//공전없이 자전만 하는 형태 == 태양
VOID SetupMatrices_1()
{
    D3DXMATRIXA16 matWorld;
    D3DXMATRIX matrotation, matscaling;

    //자전
    D3DXMatrixIdentity(&matrotation); //단위행렬화
    D3DXMatrixRotationY(&matrotation, timeGetTime() / 500.0f);

    // 크기
    D3DXMatrixScaling(&matscaling, 2.0f, 2.0f, 2.0f);

  
    matWorld = matscaling * matrotation;

    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
}

//중심을 기준으로 회전하는
//공전 반지름, 자전 속도, 공전 속도, 크기
VOID SetupMatrices_2(float p_radius, float p_rotationspeed, float p_orbitspeed, float p_size)
{
    // Set up world matrix
    D3DXMATRIXA16 matWorld;

                         //자전,공전,크기,이동
    D3DXMATRIX matrotation,matorbit, matscaling, mattranslat;

    //rotation 방식
    // 자전 (제자리에서 회전)
    D3DXMatrixRotationY(&matrotation, timeGetTime() / p_rotationspeed);
    // 이동 (공전을 위한 이동)
    D3DXMatrixTranslation(&mattranslat, p_radius, 0.0f, 0.0f);
    // 공전 (이동 후 회전)
    D3DXMatrixRotationY(&matorbit, timeGetTime() / p_orbitspeed);


    // 크기 (행성의 크기)
    D3DXMatrixScaling(&matscaling, p_size, p_size, p_size);

    // 생성된 행렬 곱하기
    // 크기,자전,이동,공전 순서
    matWorld = matscaling * matrotation * mattranslat * matorbit;

    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

    D3DXVECTOR3 vEyePt(0.0f, 30.0f, -50.0f);
    D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
    g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
    g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

//태양을 중심으로 공전하는 행성을 기준으로 공전하는 함수
//ex)지구와 달
//지구가 태양을 공전중인 반지름, 지구를 기준으로 달이 공전할 반지름, 자전 속도, 지구가 공전중인 속도, 달이 지구를 공전할 속도, 달의 크기
VOID SetupMatrices_3(float p_radius_earth ,float p_radius_moon, float p_rotationspeed , float p_orbitspeed_earth, float p_orbitspeed_moon, float p_size)
{
    // Set up world matrix
    D3DXMATRIXA16 matWorld;

    //자전,공전,크기,이동
    D3DXMATRIX matrotation, matorbit_earth, matorbit_moon, matscaling, mattranslat_earth, mattranslat_moon;

    // 달
    // 자전 (제자리에서 회전)
    D3DXMatrixRotationY(&matrotation, timeGetTime() / p_rotationspeed);
    // 이동 (공전을 위한 이동)
    D3DXMatrixTranslation(&mattranslat_moon, p_radius_moon, 0.0f, 0.0f);
    // 공전 
    D3DXMatrixRotationY(&matorbit_moon, timeGetTime() / p_orbitspeed_moon);

    //지구가 태양을 공전하는 위치로 이동 및 공전
    // 이동 
    D3DXMatrixTranslation(&mattranslat_earth, p_radius_earth, 0.0f, 0.0f);
    // 공전 
    D3DXMatrixRotationY(&matorbit_earth, timeGetTime() / p_orbitspeed_earth);

    //달 크기
    D3DXMatrixScaling(&matscaling, p_size, p_size, p_size);

    // 행렬 곱하기
    //결과 = 크기*자전*공전을위한 이동* 공전 * 지구의 공전을 위한 이동* 지구의 공전
    matWorld = matscaling * matrotation * mattranslat_moon * matorbit_moon * mattranslat_earth * matorbit_earth;
    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

    D3DXVECTOR3 vEyePt(0.0f, 30.0f, -50.0f);
    D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
    g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
    g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}


//-----------------------------------------------------------------------------
// Name: SetupLights()
// Desc: Sets up the lights and materials for the scene.
//-----------------------------------------------------------------------------
VOID SetupLights(float p_r, float p_g, float p_b)
{   // 1. 메테리얼 하나 만들고
    // 2. 이 메테리얼에 Ambient 값을 RGBA 로 세팅
    // 3. 엔진에 이 메테리얼을 쓰겠다고 알림
    // 
    // 4. 광원을 만든다
    //  광원을 하나 만들고 (D3DLIGHT9 구조체로 만든다)
    //  이 광원이 어떤 광원인지 설정해준다. (type)
    //  diffuse 멤버를 이용해 광원의 색깔을 지정해 준다. 
    // 5. 광원의 방향을 잡아준다. 
    //  벡터를 하나 만들어서
    //  벡터의 값을 세팅해서 이걸 방향값으로 사용한다. 

    // Set up a material. The material here just has the diffuse and ambient
    // colors set to yellow. Note that only one material can be used at a time.
   // srand(time(NULL));
    
    D3DMATERIAL9 mtrl;
    ZeroMemory( &mtrl, sizeof( D3DMATERIAL9 ) );
    
    mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
    mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
    mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
    g_pd3dDevice->SetMaterial( &mtrl );
    
    
    // Set up a white, directional light, with an oscillating direction.
    // Note that many lights may be active at a time (but each one slows down
    // the rendering of our scene). However, here we are just using one. Also,
    // we need to set the D3DRS_LIGHTING renderstate to enable lighting
    D3DXVECTOR3 vecDir;
    D3DLIGHT9 light;
    ZeroMemory( &light, sizeof( D3DLIGHT9 ) );
    light.Type = D3DLIGHT_DIRECTIONAL;
    //1번
    light.Diffuse.r = p_r;
    light.Diffuse.g = p_g;
    light.Diffuse.b = p_b;
    vecDir = D3DXVECTOR3( cosf( timeGetTime() / 350.0f ),
                          1.0f,
                          sinf( timeGetTime() / 350.0f ) );
    D3DXVec3Normalize( ( D3DXVECTOR3* )&light.Direction, &vecDir );
    light.Range = 1000.0f;
    g_pd3dDevice->SetLight( 0, &light );
    g_pd3dDevice->LightEnable( 0, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Finally, turn on some ambient light.
    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{

    // Clear the backbuffer and the zbuffer
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                         D3DCOLOR_XRGB( 0, 0, 255 ), 1.0f, 0 );

    // Begin the scene
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
        SetupLights(1,0,0);

        //태양
        SetupMatrices_1();
        // Render the vertex buffer contents
        g_pd3dDevice->SetStreamSource( 0, g_pVB_1, 0, sizeof( CUSTOMVERTEX ) );
        g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
        g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 * 50 - 2 );

        SetupLights(0, 1, 0);
        //지구
        SetupMatrices_2(20.0f, 500.f, 1500.0f, 1.0f);
        // Render the vertex buffer contents
        g_pd3dDevice->SetStreamSource(0, g_pVB_2, 0, sizeof(CUSTOMVERTEX));
        g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
        g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 * 50 - 2);

        SetupLights(1, 1, 0);
        //금성
        SetupMatrices_2(-10.0f, 500.0f, 300.0f , 0.4f);
        // Render the vertex buffer contents
        g_pd3dDevice->SetStreamSource(0, g_pVB_3, 0, sizeof(CUSTOMVERTEX));
        g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
        g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 * 50 - 2);

        //달
        SetupMatrices_3(20.0f, 3.0f, 500.0f, 1500.0f, 500.0f,0.3f);
        // Render the vertex buffer contents
        g_pd3dDevice->SetStreamSource(0, g_pVB_4, 0, sizeof(CUSTOMVERTEX));
        g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
        g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 * 50 - 2);

        //수성
        SetupMatrices_2(15.0f, 500.0f, 500.0f,  0.6f);
        // Render the vertex buffer contents
        g_pd3dDevice->SetStreamSource(0, g_pVB_5, 0, sizeof(CUSTOMVERTEX));
        g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
        g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 * 50 - 2);


        // End the scene
        g_pd3dDevice->EndScene();
    }

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
    UNREFERENCED_PARAMETER( hInst );

    // Register the window class
    WNDCLASSEX wc =
    {
        sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
        GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
        L"D3D Tutorial", NULL
    };
    RegisterClassEx( &wc );
    
    // Create the application's window
    HWND hWnd = CreateWindow( L"D3D Tutorial", L"D3D Planet rotation",
                              WS_OVERLAPPEDWINDOW, 100, 100, 1000, 1000,
                              NULL, NULL, wc.hInstance, NULL );

    // Initialize Direct3D
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
        // Create the geometry
        if( SUCCEEDED( InitGeometry() ) )
        {
            // Show the window
            ShowWindow( hWnd, SW_SHOWDEFAULT );
            UpdateWindow( hWnd );

            // Enter the message loop
            MSG msg;
            ZeroMemory( &msg, sizeof( msg ) );
            while( msg.message != WM_QUIT )
            {
                if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
                {

                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                }
                else
                    Render();
            }
        }
    }

    UnregisterClass( L"D3D Tutorial", wc.hInstance );
    return 0;
}



