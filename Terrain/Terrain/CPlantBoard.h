#ifndef __C_PLANT_BOARD_H__
#define __C_PLANT_BOARD_H__
#include <d3dx9.h>
#include "SVertex.h"

class CPlantBoard
{
public:
	CPlantBoard(D3DXVECTOR3 centerPos, float fWidth, float fHeight);

	~CPlantBoard();

public:
	void FlushVertexBuffer(VertexPositionTex *pVertices, int &index);
	void FlushIndexBuffer(DWORD *pIndices, int &index);

private:
	float GetRandomFloat(float min, float max)
	{
		return (min + rand() % ((int)max - (int)min + 1));
	}
private:
	D3DXVECTOR3 m_centerPos;
	float m_fWidth;
	float m_fHeight;
};
#endif