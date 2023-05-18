#pragma once

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

class CCube
{
private:
    LPDIRECT3DDEVICE9 m_pd3dDevice;
    LPDIRECT3DVERTEXBUFFER9 m_pVB;
    LPDIRECT3DINDEXBUFFER9 m_pIB;

public:
    void OnInit(LPDIRECT3DDEVICE9 pd3dDevice);
    void OnRender();
    void OnRelease();

public:
    CCube(void);
    ~CCube(void);
};