#include "skyBox.h"
#include "Convert.h"


const DWORD SkyBox::Vertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;

SkyBox::SkyBox(IDirect3DDevice9 *device, std::string topFileName, std::string bottomFileName,
	std::string leftFileName, std::string rightFileName,
	std::string frontFileName, std::string backFileName)
	:m_device(device)
{
	//加载纹理
	if (FAILED(D3DXCreateTextureFromFile(m_device, Convert::str2Wstr(frontFileName).c_str(), &m_tex[0])))
		return;
	//加载纹理
	if (FAILED(D3DXCreateTextureFromFile(m_device, Convert::str2Wstr(backFileName).c_str(), &m_tex[1])))
		return;
	//加载纹理
	if (FAILED(D3DXCreateTextureFromFile(m_device, Convert::str2Wstr(topFileName).c_str(), &m_tex[2])))
		return;
	//加载纹理
	if (FAILED(D3DXCreateTextureFromFile(m_device, Convert::str2Wstr(bottomFileName).c_str(), &m_tex[3])))
		return;
	//加载纹理
	if (FAILED(D3DXCreateTextureFromFile(m_device, Convert::str2Wstr(rightFileName).c_str(), &m_tex[4])))
		return;
	//加载纹理
	if (FAILED(D3DXCreateTextureFromFile(m_device, Convert::str2Wstr(leftFileName).c_str(), &m_tex[5])))
		return;

	// 创建顶点
	if (FAILED(m_device->CreateVertexBuffer(24 * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF,
		D3DPOOL_MANAGED, &m_vb, 0)))
		return;


	Vertex	g_Vertices[] =
	{	//Front
		{ -1.0f, 1.0f, -1.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, -1.0f, 1.0f, 0.0f },
		{ -1.0f, -1.0f, -1.0f, 0.0f, 1.0f },
		{ 1.0f, -1.0f, -1.0f,  1.0f, 1.0f },

		//Back
		{ -1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
		{ -1.0f, -1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
		{ 1.0f, -1.0f, 1.0f, 0.0f, 1.0f },

		//Up
		{ -1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f, 0.0f },
		{ -1.0f, 1.0f, -1.0f, 0.0f, 1.0f},
		{ 1.0f, 1.0f, -1.0f, 1.0f, 1.0f },

		//Down
		{ -1.0f, -1.0f, 1.0f, 0.0f, 1.0f },
		{ -1.0f, -1.0f, -1.0f, 0.0f, 0.0f},
		{ 1.0f, -1.0f, 1.0f, 1.0f, 1.0f},
		{ 1.0f, -1.0f, -1.0f, 1.0f, 0.0f},

		//Right
		{ 1.0f, 1.0f, -1.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f, 0.0f },
		{ 1.0f, -1.0f, -1.0f, 0.0f, 1.0f},
		{ 1.0f, -1.0f, 1.0f, 1.0f, 1.0f },

		//Left
		{ -1.0f, 1.0f, -1.0f, 1.0f, 0.0f },
		{ -1.0f, -1.0f, -1.0f, 1.0f, 1.0f },
		{ -1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
		{ -1.0f, -1.0f, 1.0f, 0.0f, 1.0f }

	};

	//锁住顶点缓存
	Vertex *vertex = 0;
	m_vb->Lock(0, 0, (void **)&vertex, 0);

	int scale = 1000;
	//创建front
// 	vertex[0] = Vertex(-scale, -scale, -scale, 1, 1);
// 	vertex[1] = Vertex(scale, -scale, -scale, 0, 1);
// 	vertex[2] = Vertex(scale, scale, -scale, 0, 0);
// 	vertex[3] = Vertex(-scale, scale, -scale, 1, 0);
// 
// 
// 	//创建back
// 	vertex[4] = Vertex(scale, -scale, scale, 1, 1);
// 	vertex[5] = Vertex(-scale, -scale, scale, 0, 1);
// 	vertex[6] = Vertex(-scale, scale, scale, 0, 0);
// 	vertex[7] = Vertex(scale, scale, scale, 1, 0);
// 
// 	//创建left
// 	vertex[8] = Vertex(scale, -scale, -scale, 1, 1);
// 	vertex[9] = Vertex(scale, -scale, scale, 0, 1);
// 	vertex[10] = Vertex(scale, scale, scale, 0, 0);
// 	vertex[11] = Vertex(scale, scale, -scale, 1, 0);
// 
// 	
// 
// 	//创建right
// 	vertex[12] = Vertex(-scale, -scale, scale, 1, 1);
// 	vertex[13] = Vertex(-scale, -scale, -scale, 0, 1);
// 	vertex[14] = Vertex(-scale, scale, -scale, 0, 0);
// 	vertex[15] = Vertex(-scale, scale, scale, 1, 0);
// 
// 	//创建top
// 
// 	vertex[16] = Vertex(scale, scale, -scale, 1, 1);
// 	vertex[17] = Vertex(scale, scale, scale, 1, 0);
// 	vertex[18] = Vertex(-scale, scale, scale, 0, 0);
// 	vertex[19] = Vertex(-scale, scale, -scale, 0, 1);
// 
// 
// 	//创建bottom
// 	vertex[20] = Vertex(scale, -scale, scale, 1, 1);
// 	vertex[21] = Vertex(scale, -scale, -scale, 0, 1);
// 	vertex[22] = Vertex(-scale, -scale, -scale, 0, 0);
// 	vertex[23] = Vertex(-scale, -scale, scale, 1, 0);

	for (int i = 0; i < 24; i++)
	{
		g_Vertices[i].x *= 1000;
		g_Vertices[i].y *= 1000;
		g_Vertices[i].z *= 1000;
	}
	memcpy(vertex, g_Vertices, sizeof(g_Vertices));

	m_vb->Unlock();

	if (FAILED(m_device->CreateIndexBuffer(6 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
		D3DPOOL_MANAGED, &m_ib, 0)))
		return;

	WORD *indice = 0;
	m_ib->Lock(0, 0, (void **)&indice, 0);
	indice[0] = 0;
	indice[1] = 1;
	indice[2] = 3;


	indice[3] = 0;
	indice[4] = 3;
	indice[5] = 2;
	m_ib->Unlock();
	
	m_pos = D3DXVECTOR3(0, 0, 0);
}

SkyBox::~SkyBox()
{

}

void SkyBox::Render()
{
	if (m_device)
	{
		D3DXMATRIX world;
		D3DXMatrixTranslation(&world, m_pos.x, m_pos.y, m_pos.z);

		m_device->SetTransform(D3DTS_WORLD, &world);

		m_device->SetRenderState(D3DRS_LIGHTING, false);
		m_device->SetStreamSource(0, m_vb, 0, sizeof(Vertex));
		m_device->SetFVF(Vertex::FVF);
		m_device->SetIndices(m_ib);

		m_device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		m_device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		
		for (int i = 0; i < 6; i++)
		{
			m_device->SetTexture(0, m_tex[i]);
			m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 4 * i, 0, 4, 0, 2);
		}
		m_device->SetRenderState(D3DRS_LIGHTING, true);

		
	}
}

void SkyBox::setPos(const D3DXVECTOR3 &pos)
{
	m_pos = pos;
}

void SkyBox::Update(DWORD elapsed)
{
// 	float fSpeed = elapsed / 1000.0f;
// 
// 	Vertex *vertex = 0;
// 	m_vb->Lock(0, 0, (void **)&vertex, 0);
// 
// 	vertex[8].v += fSpeed;
// 	vertex[9].v += fSpeed;
// 	vertex[10].v += fSpeed;
// 	vertex[11].v += fSpeed;
// 
// 	if (vertex[8].v > 1.1f)
// 	{
// 		vertex[8].v = 0.1f;
// 		vertex[9].v  = 0.1f;
// 		vertex[10].v = 0.1f;
// 		vertex[11].v = 0.1f;
// 	}
// 	m_vb->Unlock();
}
