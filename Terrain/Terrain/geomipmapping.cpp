#include "geomipmapping.h"
#include "math.h"

//����patch��С��ʼ��
bool GeoMipMapping::Init(int iPatchSize)
{
	if (m_iSize == 0)
		return false;

	//�ͷ���Դ
	if (m_pPatches)
		ShutDown();

	m_iPatchSize = iPatchSize;

	//patch����
	m_iNumPatchesPerSide = m_iSize / iPatchSize;

	//�����ڴ�
	m_pPatches = new SGEOMM_PATCH[m_iNumPatchesPerSide * m_iNumPatchesPerSide];
	if (m_pPatches == 0)
	{
		ShutDown();
		return false;
	}

	//��patch�ܱ���С�ָ�Ķ�
	int iDivision = m_iPatchSize - 1;
	int iLOD = 0;
	while (iDivision > 2) //Ӧ�ÿ���Ϊ1
	{
		iDivision >>= 1;
		iLOD++;
	}
	m_iMaxLOD = iLOD;

	//��ʼ��ÿ��patch
	for (int z = 0; z < m_iNumPatchesPerSide; z++)
	{
		for (int x = 0; x < m_iNumPatchesPerSide; x++)
		{
			m_pPatches[GetPatchNumber(x, z)].m_iLOD = m_iMaxLOD;  //����ϸ
		}
	}

	return true;
}

void GeoMipMapping::ShutDown()
{
	if (m_pPatches)
		delete[]m_pPatches;

	m_iPatchSize = 0;
	m_iNumPatchesPerSide = 0;
	m_iMaxLOD = 0;
}


//�����������λ�ø���LOD
void GeoMipMapping::Update(Camera *camera)
{
	float fCenterX, fCenterY, fCenterZ;

	D3DXVECTOR3 camPos = camera->getPosition();


	float dMax = 0;
	//����ÿ��patch�������䵽������ľ���
	for (int z = 0; z < m_iNumPatchesPerSide; z ++)
	{
		for (int x = 0; x < m_iNumPatchesPerSide; x++)
		{
			int iPatch = GetPatchNumber(x, z);
			fCenterX = (x * m_iPatchSize) + m_iPatchSize / 2.0f;
			fCenterZ = (z * m_iPatchSize) + m_iPatchSize / 2.0f;
			fCenterY = GetHeight(fCenterX, fCenterZ);

			//���������patch�ľ���
			m_pPatches[iPatch].m_fDistance = sqrt(Math::SQR(camPos.x - fCenterX) +
				Math::SQR(camPos.y - fCenterY) +
				Math::SQR(camPos.z - fCenterZ));

			if (m_pPatches[iPatch].m_fDistance > dMax)
				dMax = m_pPatches[iPatch].m_fDistance;

			if (m_pPatches[iPatch].m_fDistance < 20)
				m_pPatches[iPatch].m_iLOD = 0;
			else if (m_pPatches[iPatch].m_fDistance < 50)
				m_pPatches[iPatch].m_iLOD = 1;

			else if (m_pPatches[iPatch].m_fDistance < 100)
				m_pPatches[iPatch].m_iLOD = 2;

			else if (m_pPatches[iPatch].m_fDistance >= 120)
				m_pPatches[iPatch].m_iLOD = 3;
		}
	}
}



void GeoMipMapping::RenderPatch(int px, int pz)
{
	SGEOMM_NEIGHBOR patchNeighbor;
	SGEOMM_NEIGHBOR fanNeighbor;

	float fSize = 0.0f;
	float fHalfSize = 0.0f;
	float z, x;
	int iPatch = GetPatchNumber(px, pz);
	int iDivisor = 0;
	int iLOD = 0;

	//��ߵıȽϾ�ϸ�����Բ��ùܣ�ֱ�ӿ��Ի���
	if (m_pPatches[GetPatchNumber(px - 1, pz)].m_iLOD <= m_pPatches[iPatch].m_iLOD || px == 0)
		patchNeighbor.m_bLeft = true;  //���Ի���
	else
		patchNeighbor.m_bLeft = false; //���ܻ���

	if (m_pPatches[GetPatchNumber(px, pz + 1)].m_iLOD <= m_pPatches[iPatch].m_iLOD || pz == m_iNumPatchesPerSide)
		patchNeighbor.m_bUp = true;
	else
		patchNeighbor.m_bUp = false;

	if (m_pPatches[GetPatchNumber(px + 1, pz)].m_iLOD <= m_pPatches[iPatch].m_iLOD || px == m_iNumPatchesPerSide)
		patchNeighbor.m_bRight = true;
	else
		patchNeighbor.m_bRight = false;

	if (m_pPatches[GetPatchNumber(px, pz - 1)].m_iLOD <= m_pPatches[iPatch].m_iLOD || pz == 0)
		patchNeighbor.m_bBottom = true;
	else
		patchNeighbor.m_bBottom = false;

	fSize = (float)m_iPatchSize;
	iDivisor = m_iPatchSize - 1;
	iLOD = m_pPatches[iPatch].m_iLOD;

	while (iLOD-- >= 0)
		iDivisor >>= 1;

	fSize /= iDivisor;
	
	fHalfSize = fSize / 2.0f;

	for (z = fHalfSize; ((int)z + fHalfSize ) < m_iPatchSize + 1; z += fSize)
	{
		for (x = fHalfSize; ((int)x + fHalfSize) < m_iPatchSize + 1; x += fSize)
		{
			if (x == fHalfSize)
				fanNeighbor.m_bLeft = patchNeighbor.m_bLeft;
			else
				fanNeighbor.m_bLeft = true;

			if (z == fHalfSize)
				fanNeighbor.m_bBottom = patchNeighbor.m_bBottom;
			else
				fanNeighbor.m_bBottom = true;

			if (x >= (m_iPatchSize - fHalfSize))
				fanNeighbor.m_bRight = patchNeighbor.m_bRight;
			else
				fanNeighbor.m_bRight = true;

			if (z >= (m_iPatchSize - fHalfSize))
				fanNeighbor.m_bUp = patchNeighbor.m_bUp;
			else
				fanNeighbor.m_bUp = true;

			RenderFan((px * m_iPatchSize) + x, (pz * m_iPatchSize) + z, 
				fSize, fanNeighbor);
		}
	}
}

void GeoMipMapping::RenderFan(float cX, float cZ, 
	float fSize, SGEOMM_NEIGHBOR neighbor)
{
	float fHalfSize = fSize / 2.0f;

	IDirect3DVertexBuffer9 *vb = 0;

	if (FAILED(m_device->CreateVertexBuffer(10 * sizeof(TerrainVertex),
		D3DUSAGE_WRITEONLY, TerrainVertex::FVF, D3DPOOL_MANAGED, &vb, 0)))
		return; 

	int index = 0;
	//��ס����
	TerrainVertex *vertex = 0;
	if (FAILED(vb->Lock(0, 0, (void **)&vertex, 0)))
		return;

	int num = 0;
	vertex[index++] = TerrainVertex(cX, cZ, GetHeight(cX, cZ));
	vertex[index++] = TerrainVertex(cX - fHalfSize, cZ - fHalfSize, GetHeight(cX - fHalfSize, cZ - fHalfSize));
	if (neighbor.m_bLeft)
	{
		vertex[index++] = TerrainVertex(cX - fHalfSize, cZ, GetHeight(cX - fHalfSize, cZ));
		num++;
	}
	vertex[index++] = TerrainVertex(cX - fHalfSize, cZ + fHalfSize, GetHeight(cX - fHalfSize, cZ + fHalfSize));
	if (neighbor.m_bUp)
	{
		vertex[index++] = TerrainVertex(cX, cZ + fHalfSize, GetHeight(cX, cZ + fHalfSize));
		num++;
	}
	
	vertex[index++] = TerrainVertex(cX + fHalfSize, cZ + fHalfSize, GetHeight(cX + fHalfSize, cZ + fHalfSize));
	
	if (neighbor.m_bRight)
	{
		vertex[index++] = TerrainVertex(cX + fHalfSize, cZ, GetHeight(cX + fHalfSize, cZ));
		num++;
	}
	vertex[index++] = TerrainVertex(cX + fHalfSize, cZ - fHalfSize, GetHeight(cX + fHalfSize, cZ - fHalfSize));
	
	if (neighbor.m_bBottom)
	{
		vertex[index++] = TerrainVertex(cX, cZ - fHalfSize, GetHeight(cX, cZ - fHalfSize));
		num++;
	}
	vertex[index++] = TerrainVertex(cX - fHalfSize, cZ - fHalfSize, GetHeight(cX - fHalfSize, cZ - fHalfSize));
	vb->Unlock();

	if (m_device)
	{
		m_device->SetStreamSource(0, vb, 0, sizeof(TerrainVertex));
		m_device->SetFVF(TerrainVertex::FVF);

		if (m_isWireFrame)
		{
			m_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			m_device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 4 + num);
			m_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}
		else
		{
			m_device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 4 + num);
		}
		
	}
	vb->Release();
}

GeoMipMapping::GeoMipMapping(IDirect3DDevice9 *device, 
	std::string heightmapFileName, 
	int cellSpacing, float heightScale)
	:CTerrain(device, heightmapFileName, cellSpacing, heightScale)
{

}

GeoMipMapping::GeoMipMapping(IDirect3DDevice9 *device, std::string heightmapFileName, int numVertsPerRow, int numVertsPerCol, int cellSpacing, float heightScale)
	: CTerrain(device, heightmapFileName, numVertsPerRow, numVertsPerCol, cellSpacing, heightScale)
{

}

bool GeoMipMapping::Render()
{
	//��Ⱦ����patch
	for (int z = 0; z < m_iNumPatchesPerSide; z++)
	{
		for (int x = 0; x < m_iNumPatchesPerSide; x++)
		{
			RenderPatch(x, z);
		}
	}
	return true;
}

