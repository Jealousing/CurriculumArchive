#pragma once
class CCamera
{
	// 초기 카메라 세팅을 위한 좌표
	D3DXVECTOR3 m_vEye;				//카메라의 위치
	D3DXVECTOR3 m_vLookat;			//카메라가 바라보는 지점
	D3DXVECTOR3 m_vUp;					//업벡터 설정

	D3DXVECTOR3 m_vView; // 카메라가 바라보는 방향, 단위벡터
	D3DXVECTOR3 m_vCross;

	D3DXMATRIX m_matView;

public:
	CCamera();
	~CCamera();

	D3DXMATRIX* GetViewMatrix() { return &m_matView; }
	D3DXMATRIX* SetView(D3DXVECTOR3* _eye, D3DXVECTOR3* _lookat, D3DXVECTOR3* _up);
	//카메라 회전
	D3DXMATRIX* MoveLocalZ(float delta);
	D3DXMATRIX* MoveLocalX(float angle);
	D3DXMATRIX* MoveLocalY(float angle);
};

