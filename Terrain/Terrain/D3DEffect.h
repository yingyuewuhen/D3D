#pragma once
#include <d3dx9.h>
#include <string>
class D3DEffect
{
public:
	D3DEffect(IDirect3DDevice9 *device);
	virtual ~D3DEffect();

public:
	virtual bool LoadEffect(std::string fileName); //������Ч

	virtual HRESULT BeginEffect(UINT &numPass);  //����ͨ��

	virtual HRESULT EndEffect();  //�ر���Ч

	virtual void Release(); // �ͷ���Ч

public:
	ID3DXEffect *GetEffect(){ return m_pEffect; }
	
protected:
	ID3DXEffect *m_pEffect;
	IDirect3DDevice9 *m_device;

};