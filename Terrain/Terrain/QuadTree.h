#ifndef __QUAD_TREE_H__
#define __QUAD_TREE_H__

#include "terrain.h"
#include "camera.h"
#include <vector>
#include <map>


struct QuadTreeNode
{
	int m_iCenterIndex;  //中心点的索引

	int m_level;  //结点所处的层数

	int m_iIsVisible;

	QuadTreeNode *m_pChild[4];  //四个子节点
};


class QuadTree : public CTerrain
{
private:
	float m_fDesiredResolution;
	float m_fMinResolution;

	bool m_bFogIsOpen;

	int m_neighborRelative[4][2];

	std::map<int, QuadTreeNode *>m_indexMap;
	const Camera *m_camera;

	QuadTreeNode *m_root;

private:
	void DestroyQuatTree(QuadTreeNode *node);

	void CalculateIndex(QuadTreeNode *node, std::vector<DWORD> &index, int direction);

	inline int GetEdgeLegthByLevel(int level)
	{
		int iEdgeLength = m_iSize;
		for (int i = 0; i < level; i++)
		{
			iEdgeLength = (iEdgeLength + 1) / 2;
		}
		return iEdgeLength;
	}

	inline int GetIndexByEdge(int index, int edgeLength, int direction)
	{
		switch (direction)
		{
		case 0:  //上
		{
			return index + ((edgeLength - 1) / 2) * m_iSize;
			break;
		}
		case 1:  //左
		{
			return index - ((edgeLength - 1) / 2);
			break;
		}
		case 2:  //下
		{
			return index - ((edgeLength - 1) / 2) * m_iSize;
			break;
		}
		case 3: //右
		{
			return index + (edgeLength - 1) / 2;
			break;
		}
		default:
			return index;
			break;
		}
	}
	std::vector<QuadTreeNode *> GetNeighborNode(int index, int iEdgeLength);

	void RenderNode(QuadTreeNode *node);
	

	void UpdateQuatTree(QuadTreeNode *node, int index, int level, int iEdgeLength);

	
public:
	bool Init();
	void Shutdown();
	virtual bool Render();
	void UpdateQuatTree(const Camera *camera);

	void OpenFog();
	
	void CloseFog();

	QuadTree(IDirect3DDevice9 *device,
		std::string heightmapFileName,
		int cellSpacing,
		float heightScale);

	QuadTree(IDirect3DDevice9 *device,
		std::string heightmapFileName,
		int numVertsPerRow,
		int numVertsPerCol,
		int cellSpacing,
		float heightScale);


	~QuadTree();
};
#endif