#include "CTexture2D.h"
#include "Convert.h"
#include <d3d9.h>

IDirect3DDevice9 *CTexture2D::m_pDevice = 0;
CTexture2D::CTexture2D()

{

}

CTexture2D::~CTexture2D()
{

}

bool CTexture2D::LoadTexture(std::string fileName)
{
	HRESULT hr = D3DXCreateTextureFromFileEx(m_pDevice, Convert::str2Wstr(fileName).c_str(),
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		&m_imageInfo,
		0,
		&m_pTexture);
	if (FAILED(hr))
		return false;
	return true;
}
