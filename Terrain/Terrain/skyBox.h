#ifndef __SKY_BOX_H__
#define __SKY_BOX_H__

#include <d3dx9.h>
#include <d3d9.h>
#include <string>

class SkyBox
{
public:
	SkyBox(IDirect3DDevice9 *m_device, std::string topFileName, std::string bottomFileName,
		std::string leftFileName, std::string rightFileName,
		std::string frontFileName, std::string backFileName);
	~SkyBox();

	void Render();

	void Update(DWORD elapsed);
	void setPos(const D3DXVECTOR3 &pos);
private:

	struct Vertex
	{
		Vertex()
		{
		}
		Vertex(float _x, float _y, float _z,
			float _u, float _v)
			:x(_x), y(_y), z(_z), u(_u), v(_v)
		{}
		float x, y, z;
		float u, v;

		static const DWORD FVF;
	};

	IDirect3DIndexBuffer9 *m_ib;
	IDirect3DVertexBuffer9 *m_vb;

	IDirect3DDevice9 *m_device;

	unsigned int m_indices[4];
	Vertex m_vertices[24];
	IDirect3DTexture9 *m_tex[6];

	D3DXVECTOR3 m_pos;
};

#endif