#include "Rain.h"


const DWORD Rain::RAIN_VERTEX::FVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE);

Rain::Rain(IDirect3DDevice9 *device)
	:m_device(device), m_rainDirection(0, -1, 0), m_rainNum(500)
{
	Init();
}

Rain::~Rain()
{
	if (m_vb)
		m_vb->Release();
	if (m_ib)
		m_ib->Release();
	if (m_device)
		m_device->Release();
}


bool Rain::Init()
{
	//创建雨点顶点缓冲区
	if (FAILED(m_device->CreateVertexBuffer(2 * sizeof(RAIN_VERTEX), D3DUSAGE_WRITEONLY,
		RAIN_VERTEX::FVF, D3DPOOL_MANAGED, &m_vb, 0)))
		return false;

	RAIN_VERTEX *rainVertex = 0;
	m_vb->Lock(0, 0, (void **)&rainVertex, 0);
	rainVertex[0].pos = D3DXVECTOR3(1, 5, 0);
	rainVertex[0].dwColor = 0xffffffff;

	rainVertex[1].pos = D3DXVECTOR3(3, 1, 0);
	rainVertex[1].dwColor = 0xffffffff;
	m_vb->Unlock();


	m_rains.resize(m_rainNum);
	//初始化雨点数组
	for (int i = 0; i < m_rainNum; i++)
	{
		m_rains[i].x = (float)(rand() % 400 -200);
		m_rains[i].y = (float)(rand() % 400 );
		m_rains[i].z = 0.0f + rand() % 250;
		m_rains[i].fDownSpeed = 1 + rand() % 5;
	}

	m_rainDirection = D3DXVECTOR3(3, 1, 0) - D3DXVECTOR3(1, 5, 0);
	D3DXVec3Normalize(&m_rainDirection, &m_rainDirection);
	return true;
}

void Rain::Render()
{
	if (m_device)
	{
		D3DXMATRIX mat;
		
		for (std::vector<RainParticle>::iterator it = m_rains.begin();
			it != m_rains.end();
			it++)
		{
			m_device->SetStreamSource(0, m_vb, 0, sizeof(RAIN_VERTEX));
			m_device->SetFVF(RAIN_VERTEX::FVF);
			D3DXMatrixTranslation(&mat, it->x, it->y, it->z);
			m_device->SetTransform(D3DTS_WORLD, &mat);
			m_device->DrawPrimitive(D3DPT_LINELIST, 0, 1);
		}
	}
}

void Rain::Update(DWORD elapsed)
{
	//计算每个雨点的位置
// 	for (std::vector<RainParticle>::iterator it = m_rains.begin();
// 		it != m_rains.end();
// 		it++)
// 	{
// 		if (it->x > 200)
// 			it->x = -200;
// 		if (it->y < 0)
// 			it->y = 250;
// 		if (it->z > 300)
// 			it->z = (float)(rand() % 400 - 200);
// 
// 		it->x += m_rainDirection.x * it->fDownSpeed * (float)elapsed;
// 		it->y += m_rainDirection.y * it->fDownSpeed * (float)elapsed;
// 		it->z += m_rainDirection.z * it->fDownSpeed * (float)elapsed;
// 	}


	for (int i = 0; i < m_rains.size(); i++)
	{
		if (m_rains[i].x > 500)
			m_rains[i].x = -200;
		if (m_rains[i].x < -500)
			m_rains[i].x = -200;

		if (m_rains[i].y < 0)
			m_rains[i].y = 250;
		if (m_rains[i].z > 300)
			m_rains[i].z = -100;
		
		m_rains[i].x += m_rainDirection.x * m_rains[i].fDownSpeed ;
		m_rains[i].y += m_rainDirection.y * m_rains[i].fDownSpeed ;
	}
}


