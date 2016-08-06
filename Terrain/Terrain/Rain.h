#ifndef __RAIN_H__
#define __RAIN_H__
#include <vector>
#include <d3d9.h>
#include <d3dx9.h>


class Rain
{
public:
	Rain(IDirect3DDevice9 *device);
	~Rain();
	void Render();
	void Update(DWORD elapsed);
private:
	bool Init();
private:
	struct RAIN_VERTEX
	{
		D3DXVECTOR3 pos;  //
		DWORD dwColor;  //ÓêµãµÄÑÕÉ«

		const static DWORD FVF;
	};

	struct RainParticle
	{
		float x, y, z;
		float fDownSpeed;
	};
private:
	IDirect3DIndexBuffer9 *m_ib;
	IDirect3DVertexBuffer9 *m_vb;
	IDirect3DDevice9* m_device;

	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 m_rainDirection;

	int m_rainNum;

	std::vector<RainParticle> m_rains;
};
#endif