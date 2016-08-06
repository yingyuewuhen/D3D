#ifndef __QUAD_TREE_H__
#define __QUAD_TREE_H__

#include "terrain.h"
#include "camera.h"
#include <vector>
#include <map>


struct QuadTreeNode
{
	int m_iCenterIndex;  //���ĵ������

	int m_level;  //��������Ĳ���

	int m_iIsVisible;

	QuadTreeNode *m_pChild[4];  //�ĸ��ӽڵ�
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
		case 0:  //��
		{
			return index + ((edgeLength - 1) / 2) * m_iSize;
			break;
		}
		case 1:  //��
		{
			return index - ((edgeLength - 1) / 2);
			break;
		}
		case 2:  //��
		{
			return index - ((edgeLength - 1) / 2) * m_iSize;
			break;
		}
		case 3: //��
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