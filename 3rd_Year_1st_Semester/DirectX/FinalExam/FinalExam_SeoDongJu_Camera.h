#pragma once
class CCamera
{
	// �ʱ� ī�޶� ������ ���� ��ǥ
	D3DXVECTOR3 m_vEye;				//ī�޶��� ��ġ
	D3DXVECTOR3 m_vLookat;			//ī�޶� �ٶ󺸴� ����
	D3DXVECTOR3 m_vUp;					//������ ����

	D3DXVECTOR3 m_vView; // ī�޶� �ٶ󺸴� ����, ��������
	D3DXVECTOR3 m_vCross;

	D3DXMATRIX m_matView;

public:
	CCamera();
	~CCamera();

	D3DXMATRIX* GetViewMatrix() { return &m_matView; }
	D3DXMATRIX* SetView(D3DXVECTOR3* _eye, D3DXVECTOR3* _lookat, D3DXVECTOR3* _up);
	//ī�޶� ȸ��
	D3DXMATRIX* MoveLocalZ(float delta);
	D3DXMATRIX* MoveLocalX(float angle);
	D3DXMATRIX* MoveLocalY(float angle);
};

