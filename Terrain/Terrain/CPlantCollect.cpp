#include "CPlantCollect.h"

#include "QuadTree.h"
#include "Convert.h"

extern QuadTree *terrain;


CPlantCollect::CPlantCollect(IDirect3DDevice9 *device)
	:m_device(device),
	m_pPlantNodeList(NULL),
	m_pGrassTexture(NULL),
	m_pVB(NULL),
	m_pIB(NULL),
	m_fMinHeight(0),
	m_fMaxHeight(0),
	m_fDensity(0),
	m_iVertexNum(0),
	m_iIndexNum(0)
{
	m_pPlantNodeList = new std::list<CPlantBoard>();
}

CPlantCollect::~CPlantCollect()
{
	
}

bool CPlantCollect::Create(float fMinHeight, float fMaxHeight, float fdensity)
{
	m_fMinHeight = fMinHeight;
	m_fMaxHeight = fMaxHeight;
	m_fDensity = fdensity;

	if (!LoadContent())
	{
		Release();
		return false;
	}

	//����ֲ�����
	CreatePlantNode();

	//�������㻺��
	CreateVertices();

	return true;
}

void CPlantCollect::Draw()
{
	// ���ñ����޳�
	m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ����Alphaͨ��
	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// ����Alpha���
	m_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	m_device->SetRenderState(D3DRS_ALPHAREF, 150);

	// �����������㻺�����������������������ʽ
	m_device->SetTexture(0, m_pGrassTexture);
	m_device->SetStreamSource(0, m_pVB, 0, sizeof(VertexPositionTex));
	m_device->SetIndices(m_pIB);
	m_device->SetFVF(VertexPositionTex::FVF);

	// ���ƶ���
	m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iVertexNum, 0, m_iIndexNum / 3);

	// �ر�Alpha���
	m_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	// ����Alphaͨ��
	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	// ���ñ����޳�
	//m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CPlantCollect::Release()
{
	if (m_pPlantNodeList->size() > 0)
		m_pPlantNodeList->clear();

}

bool CPlantCollect::LoadContent()
{
	//��������
	HRESULT hr = D3DXCreateTextureFromFile(m_device, Convert::str2Wstr("media/grass.png").c_str(), &m_pGrassTexture);
	if (FAILED(hr))
		return false;
	return true; 
}

void CPlantCollect::CreateVertices()
{
	int nodeNum = m_pPlantNodeList->size();

	if (nodeNum == 0)
		return;

	//���㶥������
	m_iVertexNum = nodeNum * 12;
	m_iIndexNum = nodeNum * 18;

	m_device->CreateVertexBuffer(
		m_iVertexNum * sizeof(VertexPositionTex),
		D3DUSAGE_WRITEONLY,
		VertexPositionTex::FVF,
		D3DPOOL_MANAGED,
		&m_pVB,
		0);

	VertexPositionTex* pVertices;
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	int vindex = 0;
	for (std::list<CPlantBoard>::iterator ptr = m_pPlantNodeList->begin(); 
		ptr != m_pPlantNodeList->end();
		++ptr)
	{
		ptr->FlushVertexBuffer(pVertices, vindex);
	}
	m_pVB->Unlock();

	m_device->CreateIndexBuffer(
		m_iIndexNum * sizeof(DWORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&m_pIB,
		0);

	DWORD *indices = 0;
	m_pIB->Lock(0, 0, (void **)&indices, 0);

	int index = 0;
	for (std::list<CPlantBoard>::iterator ptr = m_pPlantNodeList->begin();
		ptr != m_pPlantNodeList->end();
		++ptr)
	{
		ptr->FlushIndexBuffer(indices, index);
	}

	m_pIB->Unlock();
}

void CPlantCollect::CreatePlantNode()
{
	//��������½�ص���
	for (int i = 0; i < terrain->GetSize(); i += m_fDensity)
	{
		for (int j = 0; j < terrain->GetSize(); j += m_fDensity)
		{
			float posX =  i + rand() % (int)m_fDensity;
			float posZ = j + rand() % (int)m_fDensity;

			float posY = terrain->GetHeight(posX, posZ);

			if (posY >= m_fMinHeight && posY <= m_fMaxHeight)
			{
				D3DXVECTOR3 pos = D3DXVECTOR3(posX, posY, posZ);

				POINT Size = GetPlantSize();
				pos.y += Size.y / 2;

				m_pPlantNodeList->push_back(CPlantBoard(pos, Size.x, Size.y));
			}
		}
	}
}

POINT CPlantCollect::GetPlantSize()
{
	POINT Size;
	Size.x = 16;
	Size.y = 16;
	return Size;
}
