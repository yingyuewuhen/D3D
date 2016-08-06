#pragma once
#include <d3dx9.h>
#include <string>
class D3DEffect
{
public:
	D3DEffect(IDirect3DDevice9 *device);
	virtual ~D3DEffect();

public:
	virtual bool LoadEffect(std::string fileName); //加载特效

	virtual HRESULT BeginEffect(UINT &numPass);  //开启通道

	virtual HRESULT EndEffect();  //关闭特效

	virtual void Release(); // 释放特效

public:
	ID3DXEffect *GetEffect(){ return m_pEffect; }
	
protected:
	ID3DXEffect *m_pEffect;
	IDirect3DDevice9 *m_device;

};