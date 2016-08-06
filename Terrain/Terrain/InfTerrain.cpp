#include "InfTerrain.h"


InfTerrain::InfTerrain(IDirect3DDevice9 *device, Camera *cam)
	:m_device(device), m_camera(cam)
{
	//m_terrains.resize(9);
}

InfTerrain::~InfTerrain()
{

}

bool InfTerrain::Init()
{
	m_camLogicPos = m_camera->getPosition();

	float terrainWidth = m_terrain->GetSize()-1;

	if (!m_terrain->Init())
		return false;
	
	float x = -terrainWidth, z = terrainWidth;
	for (int i = 0; i < 3; i++)
	{
		x = -terrainWidth;
		for (int j = 0; j < 3; j++)
		{
			m_terrainBlockPos.push_back(D3DXVECTOR3(x, 0, z));
			x += terrainWidth;
		}
		z -= terrainWidth;
	}

	return true;
}

void InfTerrain::Draw()
{
	if (m_device)
	{
		
		D3DXVECTOR3 oriCamPos = m_camera->getPosition();

		//�������е��ο�
		D3DXVECTOR3 pos = m_terrain->GetPosition();
		for (std::vector<D3DXVECTOR3>::iterator it = m_terrainBlockPos.begin();
			it != m_terrainBlockPos.end();
			it++)
		{
			//��ȡ����ڴ˵��ε����λ��
			float temPosX = m_camLogicPos.x - it->x;
			float temPosZ = m_camLogicPos.z - it->z;

			//�������λ�ø���view����
			m_camera->setPosition(temPosX, oriCamPos.y, temPosZ);
			
			m_device->SetTransform(D3DTS_VIEW, &m_camera->getViewMatrix());
			
			m_terrain->Render();
			
		}
		m_camera->setPosition(oriCamPos.x, oriCamPos.y, oriCamPos.z);

	}
}

void InfTerrain::Release()
{

}

void InfTerrain::Update()
{

	//��������߼����꣬ʹ����һֱ��һ�������

	m_camLogicPos = GetCameraLogicPos(m_camera, m_terrain);

	m_terrain->UpdateQuatTree(m_camera);
}

void InfTerrain::SetTerrain(QuadTree *terrain)
{
	m_terrain = terrain;
}
