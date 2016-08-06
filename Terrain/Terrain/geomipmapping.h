#ifndef __GEOMIPMAPPING_H__
#define __GEOMIPMAPPING_H__

#include "camera.h"
#include "terrain.h"
struct SGEOMM_PATCH
{
	float m_fDistance;  //patch��������ľ���
	int m_iLOD;  //patch��lod
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
	SGEOMM_PATCH *m_pPatches;  //patch����
	int m_iPatchSize;  //patch��С
	int m_iNumPatchesPerSide;  //ÿ�ߵ�patch����

	int m_iMaxLOD;  //����LOD����

	void RenderFan(float cX, float cZ, float iSize, SGEOMM_NEIGHBOR neighbor);
	void RenderPatch(int x, int z);

	inline int GetPatchNumber(int Px, int Pz)
	{
		return Pz * m_iNumPatchesPerSide + Px;
	}
public:
	bool Init(int iPatchSize);  //��ʼ��
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