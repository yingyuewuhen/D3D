#pragma once
#include <d3dx9.h>
#include "SpriteBatch.h"
#include "camera.h"
#include "CTexture2D.h"

// 光晕节点
struct FlareNode{
	float       _Position;  // 位置(以镜头中心为原点)
	float       _Scale;     // 缩放比率
	D3DXCOLOR   _Color;     // 色相
	CTexture2D* _pTexture;  // 纹理指针
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

	float m_occlusionAlpha; // 光源alpha系数

	Camera *m_camera;

	SpriteBatch *m_pSpriteBatch;
};