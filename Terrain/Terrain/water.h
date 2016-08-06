#ifndef __WATER_H__
#define __WATER_H__
#include <d3d9.h>
#include <string>
#include "CImage.h"
#include <d3dx9.h>

class Water
{
public:
	Water(IDirect3DDevice9 *device, std::string fileName);

	~Water();
	bool LoadWaterTexture(std::string fileName);

	void Render();

	void Update(DWORD elapsed);
private:
	bool Init();
private:
	IDirect3DDevice9 *m_device;
	IDirect3DVertexBuffer9 *m_vb;
	IDirect3DIndexBuffer9 *m_ib;

	IDirect3DTexture9 *m_tex;

	D3DXIMAGE_INFO m_image;

	D3DXVECTOR3 m_pos;
};
#endif