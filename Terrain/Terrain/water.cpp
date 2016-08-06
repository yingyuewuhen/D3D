#include "water.h"
#include "Convert.h"
#include "terrain.h"
Water::Water(IDirect3DDevice9 *device, std::string fileName)
	:m_device(device)
{
	LoadWaterTexture(fileName);
	Init();
}

Water::~Water()
{
	if (m_vb)
		m_vb->Release();
	if (m_ib)
		m_ib->Release();
	if (m_tex)
		m_tex->Release();
	if (m_device)
		m_device->Release();
}

bool Water::LoadWaterTexture(std::string fileName)
{
	

	if (FAILED(D3DXCreateTextureFromFileEx(m_device, Convert::str2Wstr(fileName).c_str(),
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0, 
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_RGBA(0, 0, 0, 255),
		&m_image,
		0,
		&m_tex)))
		return false;
	return true;
}

void Water::Render()
{
	if (m_device)
	{
		D3DXMATRIX mat;
		
		D3DXMatrixTranslation(&mat, m_pos.x, m_pos.y, m_pos.z);
		m_device->SetTransform(D3DTS_WORLD, &mat);

		

		//m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		m_device->SetStreamSource(0, m_vb, 0, sizeof(TerrainVertex));
		m_device->SetIndices(m_ib);
		m_device->SetTexture(0, m_tex);

		m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

		//m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	}
}

bool Water::Init()
{
	if (FAILED(m_device->CreateVertexBuffer(4 * sizeof(TerrainVertex), D3DUSAGE_WRITEONLY,
		TerrainVertex::FVF, D3DPOOL_MANAGED, &m_vb, 0)))
		return false;

	int width = 1024;
	TerrainVertex *vertex = 0;
	m_vb->Lock(0, 0, (void **)&vertex, 0);
	vertex[0] = TerrainVertex(0, 2, 0, 0, 1);
	vertex[1] = TerrainVertex(256, 2, 0, 1,1);
	vertex[2] = TerrainVertex(256, 2, 256, 1, 0);
	vertex[3] = TerrainVertex(0, 2, 256, 0, 0);
	m_vb->Unlock();


	//´´½¨Ë÷Òý
	if (FAILED(m_device->CreateIndexBuffer(6 * sizeof(WORD),
		D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED,
		&m_ib, 0)))
		return false;
	WORD *index = 0;
	m_ib->Lock(0, 0, (void **)&index, 0);
	index[0] = 0;
	index[1] = 1;
	index[2] = 2;

	index[3] = 0;
	index[4] = 2;
	index[5] = 3;
	m_ib->Unlock();
}

void Water::Update(DWORD elapsed)
{
	float fSpeed = elapsed / 100.0f;
	TerrainVertex *vertex = 0;
	m_vb->Lock(0, 0, (void **)&vertex, 0);
	vertex[0].v += fSpeed;
	vertex[1].v += fSpeed;
	vertex[2].v += fSpeed;
	vertex[3].v += fSpeed;

	if (vertex[0].v > 1.1f)
	{
		vertex[0].v = 0.1f;
		vertex[1].v = 0.1f;
		vertex[2].v = 0.1f;
		vertex[3].v = 0.1f;
	}
	m_vb->Unlock();
}
