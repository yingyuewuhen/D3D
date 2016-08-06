#include "D3DEffect.h"

#include <string>
#include "Convert.h"

D3DEffect::D3DEffect(IDirect3DDevice9 *device)
	:m_pEffect(0), m_device(device)
{

}

D3DEffect::~D3DEffect()
{

}


//加载效果文件
bool D3DEffect::LoadEffect(std::string fileName)
{
	HRESULT hr = 0;
	ID3DXBuffer *errBuffer = 0;
	D3DXCreateEffectFromFile(m_device,
		Convert::str2Wstr(fileName).c_str(),
		0,
		0,
		D3DXSHADER_DEBUG,
		0,
		&m_pEffect,
		&errBuffer);
	if (FAILED(hr))
		return false;
	return true;
}

HRESULT D3DEffect::BeginEffect(UINT &numPass)
{
	return m_pEffect->Begin(&numPass, 0);
}

HRESULT D3DEffect::EndEffect()
{
	return m_pEffect->End();
}

void D3DEffect::Release()
{
	if (m_pEffect)
		m_pEffect->Release();
}
