#ifndef __BILL_BOARD_H__
#define __BILL_BOARD_H__
#include <vector>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include "camera.h"
class BillBoard
{
public:
	BillBoard(IDirect3DDevice9 *device);
	~BillBoard();

	bool LoadTexture(std::string fileName);

	void Render();
	void Update(DWORD elapsed, const Camera &cam);

	void SetPos(const D3DXVECTOR3 &pos);
private:
	bool Init();
private:
	struct BILL_BOARD_VERTEX
	{
		BILL_BOARD_VERTEX(float _x, float _y, float _z,
			float _u, float _v)
			:x(_x), y(_y), z(_z), u(_u), v(_v)
		{}
		float x, y, z;
		float u, v;
		const static DWORD FVF;
	};
private:
	IDirect3DVertexBuffer9 *m_vb;
	IDirect3DDevice9* m_device;

	IDirect3DTexture9 *m_tex;

	D3DXVECTOR3 m_size;

	D3DXMATRIX m_mat;

	D3DXVECTOR3 m_pos;
};
#endif