#pragma once

#include <d3dx9.h>
#include <list>
#include "CTexture2D.h"

struct SpriteNode
{
	RECT _destRect;  //目标区域
	RECT _surRect;  //纹理区域

	float _layerDepth;  //z值
	
	D3DCOLOR _color;  //颜色

	SpriteNode(){}
	SpriteNode(RECT DestRect, RECT SurRect,
		float layerDepth, D3DCOLOR Color)
	{
		_destRect = DestRect;
		_surRect = SurRect;
		_layerDepth = layerDepth;
		_color = Color;
	}
};


#define SPRITEBLENDMODE_NONE            0
#define SPRITEBLENDMODE_ALPHABLEND		1
#define SPRITEBLENDMODE_ADDITIVE		2


class  SpriteBatch
{
public:
	SpriteBatch(IDirect3DDevice9 *device);
	~SpriteBatch();

public:
	void Begin(DWORD Flags);
	void End();
	void Release()
	{
		if (m_pSpriteNode->size() > 0)
			m_pSpriteNode->clear();
		delete m_pSpriteNode;

	}

	void Draw(CTexture2D *pTexture,
		const RECT &destRect,
		const RECT &surRect,
		const float &layerDepth = 0.0f,
		D3DCOLOR color = D3DCOLOR_RGBA(255, 255, 255, 255));

	void Draw(
		CTexture2D* pTexture,
		const D3DXVECTOR2& Pos,
		const D3DXVECTOR2& origin, const float& Scale,
		const float& layerDepth = 0.0f,
		D3DCOLOR Color = D3DCOLOR_RGBA(255, 255, 255, 255)
		);

	void Draw(
		CTexture2D* pTexture,
		const RECT& DesRect,
		const float& layerDepth = 0.0f,
		D3DCOLOR Color = D3DCOLOR_RGBA(255, 255, 255, 255)
		);

private:
	void PostFrame(RECT   destRect,
		RECT   surRect,
		float  layerDepth,
		D3DCOLOR color);
	void Flush();

private:
	IDirect3DDevice9 *m_pDevice;

	CTexture2D *m_pTexture;

	D3DXMATRIX m_oriViewMatrix;
	D3DXMATRIX m_oriProjMatrix;

	D3DXMATRIX m_viewMatrix;
	D3DXMATRIX m_projMatrix;

	DWORD m_Flags;

	std::list<SpriteNode> *m_pSpriteNode;
};