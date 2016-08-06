#pragma once

#include <d3dx9.h>
#include <string>

class CTexture2D
{
public:
	CTexture2D();
	~CTexture2D();

	static IDirect3DDevice9 *m_pDevice;

public:
	bool LoadTexture(std::string fileName);

	int GetWidth()
	{
		return m_imageInfo.Width;
	}
	int GetHeight()
	{
		return m_imageInfo.Height;
	}

	RECT GetRect()
	{
		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = m_imageInfo.Width;
		rect.bottom = m_imageInfo.Height;
		return rect;
	}

	IDirect3DTexture9 *GetTexture()
	{
		return m_pTexture;
	}
private:
	IDirect3DTexture9 *m_pTexture;

	D3DXIMAGE_INFO m_imageInfo;
};