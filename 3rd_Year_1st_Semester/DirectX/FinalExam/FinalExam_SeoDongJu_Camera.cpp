#include "DXUT.h"
#include "FinalExam_SeoDongJu_Camera.h"

CCamera::CCamera()
{
	D3DXVECTOR3 eye(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 lookat(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixIdentity(&m_matView);
	SetView(&eye, &lookat, &up);
}

CCamera::~CCamera()
{
}

D3DXMATRIX* CCamera::SetView(D3DXVECTOR3* _eye, D3DXVECTOR3* _lookat, D3DXVECTOR3* _up)
{
	m_vEye = *_eye;
	m_vLookat = *_lookat;
	m_vUp = *_up;

	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vLookat, &m_vUp);
	D3DXVECTOR3 in = m_vLookat - m_vEye;
	D3DXVec3Normalize(&m_vView, &in);

	return &m_matView;
}

D3DXMATRIX* CCamera::MoveLocalZ(float delta)
{
	D3DXVECTOR3 eye = m_vEye;
	D3DXVECTOR3 lookat = m_vLookat;

	D3DXVECTOR3 moveValue;
	D3DXVec3Normalize(&moveValue, &m_vView);
	moveValue *= delta;

	eye += moveValue;
	lookat += moveValue;

	return SetView(&eye, &lookat, &m_vUp);
}

D3DXMATRIX* CCamera::MoveLocalX(float delta)
{
	D3DXVECTOR3 eye = m_vEye;
	eye.y += delta;

	return SetView(&eye, &m_vLookat, &m_vUp);
}

D3DXMATRIX* CCamera::MoveLocalY(float angle)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis(&matRot, &m_vUp, angle);

	D3DXVECTOR3 moveValue;
	D3DXVec3TransformCoord(&moveValue, &m_vView, &matRot);
	moveValue += m_vEye;


	return SetView(&m_vEye, &moveValue, &m_vUp);

}
