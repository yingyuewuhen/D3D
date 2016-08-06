#include "QuadTree.h"
#include <fstream>
#include "Convert.h"
#include "math.h"


QuadTree::QuadTree(IDirect3DDevice9 *device, std::string heightmapFileName, int cellSpacing, float heightScale)
	:CTerrain(device, heightmapFileName, cellSpacing, heightScale),
	m_fMinResolution(10.0f), m_fDesiredResolution(50.0f)
{
	m_root = new QuadTreeNode;
	m_neighborRelative[0][0] = 0;
	m_neighborRelative[0][1] = 1;

	m_neighborRelative[1][0] = 3;
	m_neighborRelative[1][1] = 0;

	m_neighborRelative[2][0] = 2;
	m_neighborRelative[2][1] = 3;

	m_neighborRelative[3][0] = 1;
	m_neighborRelative[3][1] = 2;
}

QuadTree::QuadTree(IDirect3DDevice9 *device, std::string heightmapFileName, int numVertsPerRow, int numVertsPerCol, int cellSpacing, float heightScale)
	: CTerrain(device, heightmapFileName, numVertsPerRow, numVertsPerCol, cellSpacing, heightScale),
	m_fMinResolution(10.0f), m_fDesiredResolution(50.0f), m_bFogIsOpen(false)
{

}


QuadTree::~QuadTree()
{
	Shutdown();
}


bool QuadTree::Init()
{
	return true;
}

void QuadTree::Shutdown()
{
	if (m_indices.size() > 0)
		m_indices.clear();
	if (m_indexMap.size() > 0)
		m_indexMap.clear();

	if (m_root)
	{
		DestroyQuatTree(m_root);
		m_root = 0;
	}
	m_fDesiredResolution = 0;
	m_fMinResolution = 0;
}

bool QuadTree::Render()
{
	if (m_device)
	{
		D3DXMATRIX world;
		D3DXMatrixTranslation (&world, m_pos.x-1, m_pos.y, m_pos.z-1);

		m_device->SetTransform(D3DTS_WORLD, &world);

		int fCenter = (m_iSize - 1) / 2;
		RenderNode(m_root);

		if (m_indices.size() == 0)
			return false;

		m_ib->Release();
		m_device->CreateIndexBuffer(m_indices.size() * sizeof(DWORD), D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_ib, 0);

		DWORD *in = 0;
		m_ib->Lock(0, 0, (void **)&in, 0);

		memcpy(in, m_indices.data(), m_indices.size() * sizeof(DWORD));

		m_ib->Unlock();

		if (m_isWireFrame)
			m_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

		SetRenderTexture();

		//开启雾化
// 		m_device->SetRenderState(D3DRS_FOGENABLE, true);
// 
// 		//设置雾化颜色
// 		m_device->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_XRGB(255, 255, 255));
// 		m_device->SetRenderState(D3DRS_RANGEFOGENABLE, true);
// 		m_device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
// 
// 		m_device->SetRenderState(D3DRS_FOGSTART, 20);
// 		m_device->SetRenderState(D3DRS_FOGEND, 200);
// 		m_device->SetRenderState(D3DRS_LIGHTING, false);
// 
// 		m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		m_device->SetStreamSource(0, m_vb, 0, sizeof(TerrainVertex));
		m_device->SetFVF(TerrainVertex::FVF);
		m_device->SetIndices(m_ib);

		m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vertices.size(), 0, m_indices.size() / 3);

		m_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		
		m_indices.clear();
	}
	return true;
}

void QuadTree::UpdateQuatTree(const Camera *camera)
{
	m_camera = camera;
	m_indexMap.clear();
	UpdateQuatTree(m_root, m_iSize * (m_iSize - 1) / 2 + (m_iSize - 1) / 2, 0, m_iSize);
	
}

void QuadTree::UpdateQuatTree(QuadTreeNode *node, int index, int level, int iEdgeLength)
{
	int bSubdivide;

	D3DXVECTOR3 m_camLogicPos = m_camera->getPosition();
	while (m_camLogicPos.x > m_iSize)
	{
		m_camLogicPos.x -= m_iSize;
	}
	while (m_camLogicPos.x < 0)
	{
		m_camLogicPos.x += m_iSize;
	}

	while (m_camLogicPos.z > m_iSize)
	{
		m_camLogicPos.z -= m_iSize;
	}
	while (m_camLogicPos.z < 0)
	{
		m_camLogicPos.z += m_iSize;
	}

	//const D3DXVECTOR3 camPos = m_camera->getPosition();

	const D3DXVECTOR3 camPos = m_camLogicPos;
	const TerrainVertex vertex = m_vertices[index];

	//计算当前中心点到视点的距离
	float fViewDistance = (float)(fabs(camPos.x - vertex.x) +
		fabs(camPos.y - GetHeight(vertex.x, vertex.z)) +
		fabs(camPos.z - vertex.z));


	//计算f值，判断当前需不需要细分
	float fVal = fViewDistance /
		((float)iEdgeLength * m_fMinResolution * Math::MAX(0, 1.0f));

	if (fVal < 1.0f)
		bSubdivide = 1;
	else
		bSubdivide = 0;

	node->m_iIsVisible = bSubdivide;
	node->m_level = level;
	node->m_iCenterIndex = index;
	
	m_indexMap[index] = node;

	for (int i = 0; i < 4; i++)
	{
		node->m_pChild[i] = 0;
	}
	
	if (iEdgeLength <= 3)
	{
		node->m_iIsVisible = 0;
		return;
	}

	//判断是否在相机里可见

	if (1 == node->m_iIsVisible)
	{
		for (int i = 0; i < 4; i++)
		{
			if (!node->m_pChild[i])
				node->m_pChild[i] = new QuadTreeNode;
		}
		int iChildLength = (iEdgeLength + 1) >> 1;
		//计算左上的索引
		int leftUpIndex = index + m_iSize * ((iChildLength - 1) / 2) - (iChildLength - 1) / 2;
		UpdateQuatTree(node->m_pChild[0], leftUpIndex, level + 1, iChildLength);

		int rightUpIndex = index + m_iSize * ((iChildLength - 1) / 2) + (iChildLength - 1) / 2;
		UpdateQuatTree(node->m_pChild[1], rightUpIndex, level + 1, iChildLength);

		int rightDownIndex = index - m_iSize * ((iChildLength - 1) / 2) + (iChildLength - 1) / 2;
		UpdateQuatTree(node->m_pChild[2], rightDownIndex, level + 1, iChildLength);

		int leftDownIndex = index - m_iSize * ((iChildLength - 1) / 2) - (iChildLength - 1) / 2;

		UpdateQuatTree(node->m_pChild[3], leftDownIndex, level + 1, iChildLength);
	}
	
}

void QuadTree::RenderNode(QuadTreeNode *node)
{
	if (node == 0)
		return;

	if (node->m_iIsVisible)
	{
		//分别渲染四个子节点
		for (int i = 0; i < 4; i++)
		{
			RenderNode(node->m_pChild[i]);
		}
	}
	else //渲染此节点
	{
		

		int iEdgeLength = GetEdgeLegthByLevel(node->m_level);
		
		//通过当前节点的index和edgeSize计算出相邻level的node
		std::vector<QuadTreeNode *>neighborNode = GetNeighborNode(node->m_iCenterIndex, iEdgeLength);
		
		
		std::vector<DWORD> temIndex;

		int iOffset = (iEdgeLength - 1) / 2;
		temIndex.push_back(node->m_iCenterIndex);
		temIndex.push_back(node->m_iCenterIndex + (iOffset * m_iSize) + iOffset);  //上右端点
		
		if (neighborNode[0])
			CalculateIndex(neighborNode[0], temIndex, 2);
		else
			temIndex.push_back(node->m_iCenterIndex + (iOffset * m_iSize));

		temIndex.push_back(node->m_iCenterIndex + (iOffset * m_iSize) - iOffset);
		
		if (neighborNode[1])
			CalculateIndex(neighborNode[1], temIndex, 3);
		else
			temIndex.push_back(node->m_iCenterIndex - iOffset);

		temIndex.push_back(node->m_iCenterIndex - (iOffset * m_iSize) - iOffset);

		if (neighborNode[2])
			CalculateIndex(neighborNode[2], temIndex, 0);
		else
			temIndex.push_back(node->m_iCenterIndex - (iOffset * m_iSize));
		temIndex.push_back(node->m_iCenterIndex - (iOffset * m_iSize) + iOffset);

		if (neighborNode[3])
			CalculateIndex(neighborNode[3], temIndex, 1);
		else
			temIndex.push_back(node->m_iCenterIndex + iOffset);

		for (int i = 0; i < temIndex.size() - 2; i++)
		{
			m_indices.push_back(temIndex[0]);
			m_indices.push_back(temIndex[(i + 1)]);
			m_indices.push_back(temIndex[(i + 2)]);
		}
		m_indices.push_back(temIndex[0]);
		m_indices.push_back(temIndex[temIndex.size() - 1]);
		m_indices.push_back(temIndex[1]);

		//判断相邻的兄弟节点是否可分
		
	}
}



std::vector<QuadTreeNode *> QuadTree::GetNeighborNode(int index, int iEdgeLength)
{
	//计算右边的索引
	int rightIndex = index + (iEdgeLength - 1);
	int upIndex = index + (iEdgeLength - 1) * m_iSize;
	int leftIndex = index - (iEdgeLength - 1);
	int downIndex = index - (iEdgeLength - 1) * m_iSize;

	std::vector<QuadTreeNode *> temNode(4);

	int row = index / m_iSize;

	if (upIndex < m_iSize * m_iSize)
		temNode[0] = (m_indexMap[upIndex]);

	if (leftIndex / m_iSize == row)
		temNode[1] = (m_indexMap[leftIndex]);

	if (downIndex >= 0)
		temNode[2] = (m_indexMap[downIndex]);

	if (rightIndex / m_iSize == row)
		temNode[3] = (m_indexMap[rightIndex]);
	
	return temNode;
}

void QuadTree::CalculateIndex(QuadTreeNode *node, std::vector<DWORD> &index, int direction)
{
	if (!node->m_iIsVisible)  //添加索引
	{
		//计算索引位置
		int iEdgeLegth = GetEdgeLegthByLevel(node->m_level);

		index.push_back(GetIndexByEdge(node->m_iCenterIndex, iEdgeLegth, direction));
		return;
	}

	if (node->m_pChild[m_neighborRelative[direction][0]])
	{
		CalculateIndex(node->m_pChild[m_neighborRelative[direction][0]], index, direction);
	}
	//添加中间索引
	index.push_back(GetIndexByEdge(node->m_iCenterIndex, GetEdgeLegthByLevel(node->m_level), direction));

	if (node->m_pChild[m_neighborRelative[direction][1]])
	{
		CalculateIndex(node->m_pChild[m_neighborRelative[direction][1]], index, direction);
	}
}

void QuadTree::DestroyQuatTree(QuadTreeNode *node)
{
	if (node == 0)
		return;
	for (int i = 0; i < 4; i++)
	{
		DestroyQuatTree(node->m_pChild[i]);
	}
	delete node;
}
