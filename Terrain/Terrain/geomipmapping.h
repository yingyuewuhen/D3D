#ifndef __GEOMIPMAPPING_H__
#define __GEOMIPMAPPING_H__

#include "camera.h"
#include "terrain.h"
struct SGEOMM_PATCH
{
	float m_fDistance;  //patch到摄像机的距离
	int m_iLOD;  //patch的lod
};

struct SGEOMM_NEIGHBOR
{
	bool m_bLeft;
	bool m_bRight;
	bool m_bUp;
	bool m_bBottom;
};


class GeoMipMapping :public CTerrain
{
private:
	SGEOMM_PATCH *m_pPatches;  //patch数据
	int m_iPatchSize;  //patch大小
	int m_iNumPatchesPerSide;  //每边的patch数量

	int m_iMaxLOD;  //最大的LOD层数

	void RenderFan(float cX, float cZ, float iSize, SGEOMM_NEIGHBOR neighbor);
	void RenderPatch(int x, int z);

	inline int GetPatchNumber(int Px, int Pz)
	{
		return Pz * m_iNumPatchesPerSide + Px;
	}
public:
	bool Init(int iPatchSize);  //初始化
	void ShutDown();
	void Update(Camera *camera);
	virtual bool Render();

	GeoMipMapping(IDirect3DDevice9 *device,
		std::string heightmapFileName,
		int cellSpacing,
		float heightScale);
	GeoMipMapping(IDirect3DDevice9 *device,
		std::string heightmapFileName,
		int numVertsPerRow,
		int numVertsPerCol,
		int cellSpacing,
		float heightScale);

	~GeoMipMapping();

};
#endif