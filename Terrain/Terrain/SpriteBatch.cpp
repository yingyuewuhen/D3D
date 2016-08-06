#include "SpriteBatch.h"
#include <vector>

struct VertexSprite{
	VertexSprite(){}
	VertexSprite(float x, float y, float z, float nx, float ny, float nz, D3DCOLOR color, float u, float v){
		_x = x; _y = y; _z = z;
		_nx = nx; _ny = ny; _nz = nz;
		_color = color;
		_u = u; _v = v;
	}
	float _x, _y, _z;
	float _nx, _ny, _nz;
	D3DCOLOR _color;
	float _u, _v;
	static const DWORD FVF;
};

const DWORD VertexSprite::FVF = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1);


SpriteBatch::SpriteBatch(IDirect3DDevice9 *device)
	:m_pDevice(device)
{
	D3DXMatrixIdentity(&m_viewMatrix);

	//获取正交投影矩阵
	D3DXMatrixOrthoOffCenterLH(&m_projMatrix, 0.0f, 800, 600, 0.0f, 0.0f, 1.0f);

	//初始化点精灵列表
	m_pSpriteNode = new std::list<SpriteNode>();

	m_Flags = SPRITEBLENDMODE_NONE;
}

SpriteBatch::~SpriteBatch()
{

}

void SpriteBatch::Begin(DWORD Flags)
{
	//获取原始状态
	m_pDevice->GetTransform(D3DTS_VIEW, &m_oriViewMatrix);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &m_oriProjMatrix);

	//设置单位摄影矩阵和正交投影
	m_pDevice->SetTransform(D3DTS_VIEW, &m_viewMatrix);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &m_projMatrix);

	m_Flags = Flags;

	if (Flags == SPRITEBLENDMODE_ALPHABLEND)
	{
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
	// 如果是ADDITIVE模式
	else if (Flags == SPRITEBLENDMODE_ADDITIVE)
	{
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}
}

void SpriteBatch::End()
{
	//还原之前绘制所有结点
	Flush();

	if (m_Flags != SPRITEBLENDMODE_NONE)
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	//还原矩阵
	m_pDevice->SetTransform(D3DTS_VIEW, &m_oriViewMatrix);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &m_oriProjMatrix);
}


void SpriteBatch::Draw(CTexture2D *pTexture,
	const RECT &destRect, 
	const RECT &surRect, const float &layerDepth /*= 0.0f*/, D3DCOLOR color /*= D3DCOLOR_RGBA(255, 255, 255, 255)*/)
{
	if (0 == m_pTexture)
		m_pTexture = pTexture;

	if (m_pTexture != pTexture)
	{
		Flush();
		m_pTexture = pTexture;
	}

	PostFrame(destRect, surRect, layerDepth, color);
}

void SpriteBatch::Draw(CTexture2D* pTexture, 
	const D3DXVECTOR2& Pos, 
	const D3DXVECTOR2& origin, const float& Scale, 
	const float& layerDepth /*= 0.0f*/, D3DCOLOR Color /*= D3DCOLOR_RGBA(255, 255, 255, 255) */)
{
	D3DXVECTOR2 offset = origin * Scale;
	RECT rect;
	rect.left = Pos.x - offset.x;
	rect.top = Pos.y - offset.y;
	rect.right = rect.left + pTexture->GetWidth()  * Scale;
	rect.bottom = rect.top + pTexture->GetHeight() * Scale;
	Draw(pTexture, rect, layerDepth, Color);
}

void SpriteBatch::Draw(CTexture2D* pTexture, const RECT& DesRect, const float& layerDepth /*= 0.0f*/, D3DCOLOR Color /*= D3DCOLOR_RGBA(255, 255, 255, 255) */)
{
	Draw(pTexture, DesRect, pTexture->GetRect(), layerDepth, Color);
}


//增加一个渲染结点
void SpriteBatch::PostFrame(RECT destRect, RECT surRect, float layerDepth, D3DCOLOR color)
{
	m_pSpriteNode->push_back(SpriteNode(destRect, surRect, layerDepth, color));
}


//渲染所有结点
void SpriteBatch::Flush()
{
	int numNodes = m_pSpriteNode->size();
	if (m_pSpriteNode->size() <= 0 || !m_pTexture)
		return;

	//申请顶点和索引缓冲区
	std::vector<VertexSprite>vb(numNodes * 4);

	//VertexSprite *vb = new VertexSprite[numNodes * 4];
	WORD *ib = new WORD[numNodes * 6];

	//memset(vb, 0, sizeof(SpriteNode) * 4 * numNodes);
	memset(ib, 0, sizeof(WORD) * numNodes * 4);


	int i = 0;
	//遍历所有顶点
	for (std::list<SpriteNode>::iterator ptr = m_pSpriteNode->begin();
		ptr != m_pSpriteNode->end();
		ptr++)
	{
		float Txcrd_LU_u = ptr->_surRect.left / m_pTexture->GetWidth();
		float Txcrd_LU_v = ptr->_surRect.top / m_pTexture->GetHeight();

		float Txcrd_RU_u = ptr->_surRect.right / m_pTexture->GetWidth();
		float Txcrd_RU_v = ptr->_surRect.top / m_pTexture->GetHeight();

		float Txcrd_RD_u = ptr->_surRect.right / m_pTexture->GetWidth();
		float Txcrd_RD_v = ptr->_surRect.bottom / m_pTexture->GetHeight();

		float Txcrd_LD_u = ptr->_surRect.left / m_pTexture->GetWidth();
		float Txcrd_LD_v = ptr->_surRect.bottom / m_pTexture->GetHeight();


		vb[i * 4] = VertexSprite(ptr->_destRect.left, ptr->_destRect.top, ptr->_layerDepth, 0.0f, 0.0f, 0.0f, ptr->_color, Txcrd_LU_u, Txcrd_LU_v);
		vb[i * 4 + 1] = VertexSprite(ptr->_destRect.right, ptr->_destRect.top, ptr->_layerDepth, 0.0f, 0.0f, 0.0f, ptr->_color, Txcrd_RU_u, Txcrd_RU_v);
		vb[i * 4 + 2] = VertexSprite(ptr->_destRect.right, ptr->_destRect.bottom, ptr->_layerDepth, 0.0f, 0.0f, 0.0f, ptr->_color, Txcrd_RD_u, Txcrd_RD_v);
		vb[i * 4 + 3] = VertexSprite(ptr->_destRect.left, ptr->_destRect.bottom, ptr->_layerDepth, 0.0f, 0.0f, 0.0f, ptr->_color, Txcrd_LD_u, Txcrd_LD_v);

		//填充索引

		ib[i * 6] = i * 4;
		ib[i * 6 + 1] = i * 4 + 1;
		ib[i * 6 + 2] = i * 4 + 2;

		ib[i * 6 + 3] = i * 4;
		ib[i * 6 + 4] = i * 4 + 2;
		ib[i * 6 + 5] = i * 4 + 3;
		i++;
	}

	//设置纹理
	m_pDevice->SetTexture(0, m_pTexture->GetTexture());

	m_pDevice->SetFVF(VertexSprite::FVF);
	m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, numNodes * 4,
		numNodes * 2, &ib[0],
		D3DFMT_INDEX16, &vb[0], sizeof(VertexSprite));

	vb.clear();
	delete[]ib;
	

	m_pSpriteNode->clear();
}
