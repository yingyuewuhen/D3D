#pragma once
#include <d3dx9.h>
#include "SpriteBatch.h"
#include "camera.h"
#include "CTexture2D.h"

// ���νڵ�
struct FlareNode{
	float       _Position;  // λ��(�Ծ�ͷ����Ϊԭ��)
	float       _Scale;     // ���ű���
	D3DXCOLOR   _Color;     // ɫ��
	CTexture2D* _pTexture;  // ����ָ��
	FlareNode(){}
	FlareNode(float Position, float Scale, D3DXCOLOR Color, CTexture2D* pTexture){
		_Position = Position;  _Scale = Scale;  _Color = Color;  _pTexture = pTexture;
	};
};



class LensFlare
{
public:
	LensFlare(IDirect3DDevice9 *device, Camera *cam);
	~LensFlare();

public:
	bool Create(D3DXVECTOR3 lightPos, D3DXVECTOR2 viewPortSize);

	void Draw();

	void Release();

private:
	void CalRelativeLightPos(int &iCoordX, int &iCoordY, int &iCoordW);

	bool LoadContent();

	bool IsVisible();

private:
	IDirect3DDevice9 *m_pDevice;
	CTexture2D *m_pTex1;
	CTexture2D *m_pTex2;
	CTexture2D *m_pTex3;
	CTexture2D *m_pGlowTex;

private:
	D3DXVECTOR3 m_lightPosition;
	D3DXVECTOR2 m_relativeLightPos;
	D3DXVECTOR2 m_viewPortCenter;

	FlareNode m_flareNodes[10];

	float m_occlusionAlpha; // ��Դalphaϵ��

	Camera *m_camera;

	SpriteBatch *m_pSpriteBatch;
};