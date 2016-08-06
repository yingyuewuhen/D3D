#pragma once
#include "terrain.h"
#include "camera.h"
#include "QuadTree.h"

class InfTerrain
{
public:
	InfTerrain(IDirect3DDevice9 *device, Camera *cam);
	~InfTerrain();
	
	void SetTerrain(QuadTree *terrain);
	bool Init(); //初始化

	void Draw(); //绘制无限地形

	void Update();

	void Release();

	D3DXVECTOR3 GetCameraLogicPos(Camera *cam, QuadTree *terrain) const
	{
		D3DXVECTOR3 camLogicPos = cam->getPosition();
		int terrainWidth = terrain->GetSize();
		while (camLogicPos.x > terrainWidth)
		{
			camLogicPos.x -= terrainWidth;
		}
		while (camLogicPos.x < 0)
		{
			camLogicPos.x += terrainWidth;
		}

		while (camLogicPos.z > terrainWidth)
		{
			camLogicPos.z -= terrainWidth;
		}
		while (camLogicPos.z < 0)
		{
			camLogicPos.z += terrainWidth;
		}
		return camLogicPos;
	}
private:

	QuadTree * m_terrain;

	std::vector<D3DXVECTOR3> m_terrainBlockPos;
	IDirect3DDevice9 *m_device;
	D3DXVECTOR3 m_camLogicPos;
	Camera *m_camera;
};