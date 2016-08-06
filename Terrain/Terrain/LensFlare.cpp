#include "LensFlare.h"
#include "Convert.h"

LensFlare::LensFlare(IDirect3DDevice9 *device, Camera *cam)
	:m_pDevice(device), m_camera(cam),
	m_pTex1(0), m_pTex2(0),
	m_pTex3(0), m_pGlowTex(0)
{

	m_pSpriteBatch = new SpriteBatch(device);

	CTexture2D::m_pDevice = device;
	m_pTex1 = new CTexture2D;
	m_pTex2 = new CTexture2D;
	m_pTex3 = new CTexture2D;
	m_pGlowTex = new CTexture2D;

}

LensFlare::~LensFlare()
{

}

bool LensFlare::Create(D3DXVECTOR3 lightPos, D3DXVECTOR2 viewPortSize)
{
	m_lightPosition = lightPos;

	m_viewPortCenter = D3DXVECTOR2(viewPortSize.x / 2, viewPortSize.y / 2);

	if (!LoadContent())
		return false;
	return true;
}

void LensFlare::Draw()
{
	if (IsVisible())
	{
		m_pSpriteBatch->Begin(SPRITEBLENDMODE_ALPHABLEND);

		D3DXCOLOR color = D3DCOLOR_COLORVALUE(1, 1, 1, m_occlusionAlpha);

		D3DXVECTOR2 glowOrigin = D3DXVECTOR2(m_pGlowTex->GetWidth(), m_pGlowTex->GetHeight()) / 2;
		float scale = 1;

		
		m_pSpriteBatch->Draw(m_pGlowTex, m_relativeLightPos, glowOrigin, scale);
		m_pSpriteBatch->End();


		m_pSpriteBatch->Begin(SPRITEBLENDMODE_ADDITIVE);
		D3DXVECTOR2 flareVector = m_viewPortCenter - m_relativeLightPos;

		for (int i = 0; i < 10; i++)
		{
			D3DXVECTOR2 flarePos = m_relativeLightPos + flareVector * m_flareNodes[i]._Position;
			D3DXVECTOR2 flareOrgin = D3DXVECTOR2(m_flareNodes[i]._pTexture->GetWidth(), m_flareNodes[i]._pTexture->GetHeight()) / 2;
			m_pSpriteBatch->Draw(m_flareNodes[i]._pTexture, flarePos, flareOrgin, 1);
		}
		m_pSpriteBatch->End();
	}
}

void LensFlare::Release()
{

}

void LensFlare::CalRelativeLightPos(int &iCoordX, int &iCoordY, int &iCoordW)
{
	// 计算光源在2D屏幕中的相对位置，固定公式
	D3DXMATRIX matWorld, matView, matConcat, matViewportScale;
	D3DXVECTOR4 vResult;

	matViewportScale = D3DXMATRIX(
		m_viewPortCenter.x, 0, 0, 0,
		0, -m_viewPortCenter.y, 0, 0,
		0, 0, 1, 0,
		m_viewPortCenter.x, m_viewPortCenter.y, 0, 1
		);

	matView = m_camera->getViewMatrix();
	D3DXMatrixIdentity(&matWorld);
	matConcat = matWorld;
	matConcat *= matView;
	matConcat *= m_camera->getProjMatrix();
	matConcat *= matViewportScale;

	D3DXVECTOR3 resultLightPos = m_lightPosition;// +m_camera->getPosition();
	D3DXVec3Transform(&vResult, &resultLightPos, &matConcat);

	iCoordX = vResult.x / vResult.w;
	iCoordY = vResult.y / vResult.w;
	iCoordW = vResult.w;
}

bool LensFlare::LoadContent()
{

	if (!m_pTex1->LoadTexture("media/flare1.png"))
		return false;

	if (!m_pTex2->LoadTexture("media/flare2.png"))
		return false;

	if (!m_pTex3->LoadTexture("media/flare3.png"))
		return false;

	if (!m_pGlowTex->LoadTexture("media/glow.png"))
		return false;

	//初始化光晕结点
	int i = 0;
	m_flareNodes[i++] = FlareNode(-0.5f, 0.7f, D3DCOLOR_XRGB(50, 25, 50), m_pTex1);
	m_flareNodes[i++] = FlareNode(0.3f, 0.4f, D3DCOLOR_XRGB(100, 225, 200), m_pTex1);
	m_flareNodes[i++] = FlareNode(1.2f, 1.0f, D3DCOLOR_XRGB(100, 50, 50), m_pTex1);
	m_flareNodes[i++] = FlareNode(1.5f, 1.5f, D3DCOLOR_XRGB(50, 100, 50), m_pTex1);

	m_flareNodes[i++] = FlareNode(-0.3f, 0.7f, D3DCOLOR_XRGB(200, 50, 50), m_pTex2);
	m_flareNodes[i++] = FlareNode(0.6f, 0.9f, D3DCOLOR_XRGB(50, 100, 50), m_pTex2);
	m_flareNodes[i++] = FlareNode(0.7f, 0.4f, D3DCOLOR_XRGB(50, 200, 200), m_pTex2);

	m_flareNodes[i++] = FlareNode(-0.7f, 0.7f, D3DCOLOR_XRGB(50, 100, 25), m_pTex3);
	m_flareNodes[i++] = FlareNode(0.0f, 0.6f, D3DCOLOR_XRGB(25, 25, 25), m_pTex3);
	m_flareNodes[i++] = FlareNode(2.0f, 1.4f, D3DCOLOR_XRGB(25, 50, 100), m_pTex3);

	return true;

}

bool LensFlare::IsVisible()
{
	// 判断光源是否可见(目前仅粗略判断)
	int X, Y, W = 0;
	CalRelativeLightPos(X, Y, W);
	if (W > 0.0f &&
		X >= -100 && X < m_viewPortCenter.x * 2 + 100 &&
		Y >= -100 && Y < m_viewPortCenter.y * 2 + 100)
	{
		m_relativeLightPos.x = X;
		m_relativeLightPos.y = Y;
		m_occlusionAlpha = W;
		return true;
	}
	else
		return false;
}
