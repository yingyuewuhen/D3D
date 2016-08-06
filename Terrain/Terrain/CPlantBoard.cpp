#include "CPlantBoard.h"
#include <d3dx9math.h>

CPlantBoard::CPlantBoard(D3DXVECTOR3 centerPos, float fWidth, float fHeight)
	:m_centerPos(centerPos), m_fWidth(fWidth), m_fHeight(fHeight)
{

}

CPlantBoard::~CPlantBoard()
{

}

void CPlantBoard::FlushVertexBuffer(VertexPositionTex *pVertices, int &index)
{
	//��һ���棬������λ��

	D3DXVECTOR3 pos_LU = D3DXVECTOR3(- m_fWidth / 2.0f, m_fHeight / 2.0f, 0);
	D3DXVECTOR3 pos_RU = D3DXVECTOR3(m_fWidth / 2.0f, m_fHeight / 2.0f, 0);
	D3DXVECTOR3 pos_LB = D3DXVECTOR3(-m_fWidth / 2.0f, -m_fHeight / 2.0f, 0);
	D3DXVECTOR3 pos_RB = D3DXVECTOR3(m_fWidth / 2.0f, -m_fHeight / 2.0f, 0);

	//������������
	D3DXVECTOR2 uv_LU = D3DXVECTOR2(0, 0);
	D3DXVECTOR2 uv_RU = D3DXVECTOR2(1, 0);
	D3DXVECTOR2 uv_LB = D3DXVECTOR2(0, 1);
	D3DXVECTOR2 uv_RB = D3DXVECTOR2(1, 1);


	//���������ƫ��
	float randomAngle = GetRandomFloat(D3DX_PI / 4.0f, D3DX_PI * 5.0f / 12.0f);
	D3DXMATRIX rotateMatrix;  //��ת����
	D3DXMatrixRotationY(&rotateMatrix, randomAngle);

	D3DXMATRIX rotateMatrix2;
	D3DXMatrixRotationY(&rotateMatrix2, D3DX_PI - randomAngle);


	//�ڶ������ƫ��
	D3DXVECTOR3 pos_LU2;
	D3DXVECTOR3 pos_RU2;
	D3DXVECTOR3 pos_LB2;
	D3DXVECTOR3 pos_RB2;

	//�Ը����������ת
	D3DXVec3TransformCoord(&pos_LU2, &pos_LU, &rotateMatrix);
	D3DXVec3TransformCoord(&pos_RU2, &pos_RU, &rotateMatrix);
	D3DXVec3TransformCoord(&pos_LB2, &pos_LB, &rotateMatrix);
	D3DXVec3TransformCoord(&pos_RB2, &pos_RB, &rotateMatrix);


	//���ɵ�������
	D3DXVECTOR3 pos_LU3;
	D3DXVECTOR3 pos_RU3;
	D3DXVECTOR3 pos_LB3;
	D3DXVECTOR3 pos_RB3;

	D3DXVec3TransformCoord(&pos_LU3, &pos_LU, &rotateMatrix2);
	D3DXVec3TransformCoord(&pos_RU3, &pos_RU, &rotateMatrix2);
	D3DXVec3TransformCoord(&pos_LB3, &pos_LB, &rotateMatrix2);
	D3DXVec3TransformCoord(&pos_RB3, &pos_RB, &rotateMatrix2);


	//���������ݱ��浽������
	pVertices[index++] = VertexPositionTex(m_centerPos + pos_LU, uv_LU);
	pVertices[index++] = VertexPositionTex(m_centerPos + pos_RU, uv_RU);
	pVertices[index++] = VertexPositionTex(m_centerPos + pos_LB, uv_LB);
	pVertices[index++] = VertexPositionTex(m_centerPos + pos_RB, uv_RB);

	pVertices[index++] = VertexPositionTex(m_centerPos + pos_LU2, uv_LU);
	pVertices[index++] = VertexPositionTex(m_centerPos + pos_RU2, uv_RU);
	pVertices[index++] = VertexPositionTex(m_centerPos + pos_LB2, uv_LB);
	pVertices[index++] = VertexPositionTex(m_centerPos + pos_RB2, uv_RB);

	pVertices[index++] = VertexPositionTex(m_centerPos + pos_LU2, uv_LU);
	pVertices[index++] = VertexPositionTex(m_centerPos + pos_RU2, uv_RU);
	pVertices[index++] = VertexPositionTex(m_centerPos + pos_LB2, uv_LB);
	pVertices[index++] = VertexPositionTex(m_centerPos + pos_RB2, uv_RB);
}

void CPlantBoard::FlushIndexBuffer(DWORD *pIndices, int &index)
{
	DWORD vindex = (DWORD)index * 2 / 3;

	// ���л���һ���������
	pIndices[index++] = vindex;
	pIndices[index++] = vindex + 1;
	pIndices[index++] = vindex + 2;

	pIndices[index++] = vindex + 1;
	pIndices[index++] = vindex + 3;
	pIndices[index++] = vindex + 2;

	// ���л��ڶ����������
	pIndices[index++] = vindex + 4;
	pIndices[index++] = vindex + 5;
	pIndices[index++] = vindex + 6;

	pIndices[index++] = vindex + 5;
	pIndices[index++] = vindex + 7;
	pIndices[index++] = vindex + 6;

	// ���л��������������
	pIndices[index++] = vindex + 8;
	pIndices[index++] = vindex + 9;
	pIndices[index++] = vindex + 10;

	pIndices[index++] = vindex + 9;
	pIndices[index++] = vindex + 11;
	pIndices[index++] = vindex + 10;
}
