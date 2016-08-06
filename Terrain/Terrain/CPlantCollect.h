#pragma  once

#include <list>
#include "CPlantBoard.h"
class CPlantCollect
{
public:
	CPlantCollect(IDirect3DDevice9 *device);
	~CPlantCollect();

public:
	bool Create(float fMinHeight,
		float fMaxHeight,
		float fdensity);

	void Draw();
	void Release();

private:
	bool LoadContent();
	void CreateVertices();
	void CreatePlantNode();
	POINT GetPlantSize();

private:
	std::list<CPlantBoard>*m_pPlantNodeList;
	IDirect3DTexture9 *m_pGrassTexture;
	IDirect3DVertexBuffer9 *m_pVB;
	IDirect3DIndexBuffer9 *m_pIB;
	IDirect3DDevice9 *m_device;
private:
	float m_fMinHeight;
	float m_fMaxHeight;
	float m_fDensity;
	int m_iVertexNum;
	int m_iIndexNum;
};