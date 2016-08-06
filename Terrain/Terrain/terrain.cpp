
#include "Terrain.h"

#include "Convert.h"
#include <string>
#include <fstream>
#include "math.h"

using std::string;
using std::ifstream;
using std::vector;

const DWORD TerrainVertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEX2;


CTerrain::CTerrain(IDirect3DDevice9 *device, 
	std::string heightmapFileName, 
	int numVertsPerRow, int numVertsPerCol, 
	int cellSpacing, float heightScale)
	:m_device(device), m_tex(0), m_texture(0), m_iDetailRepeat(10)
{
	m_numVertsPerCol = numVertsPerCol;
	m_numVertsPerRow = numVertsPerRow;
	m_cellSpacing = cellSpacing;

	m_numCellsPerCol = m_numVertsPerCol - 1;
	m_numCellsPerRow = m_numVertsPerRow - 1;


	//����
	m_width = m_numCellsPerRow * m_cellSpacing;
	m_height = m_numCellsPerCol * m_cellSpacing;

	//�������
	m_numVertices = m_numVertsPerRow * m_numVertsPerCol;

	//�����εĸ���
	m_numTriangles = m_numCellsPerCol * m_numCellsPerRow * 2;

	m_heightScale = heightScale;

	//���ظ߶�ͼ
	if (!ReadRawFile(heightmapFileName))
		return;

	//�Ը߶Ƚ���scale
	for (int i = 0; i < m_heightMap.size(); i++)
		m_heightMap[i] *= m_heightScale;

	if (!ComputeVertices())
		return;

	if (!ComputeIndices())
		return;

	m_texture = new CImage();
	if (m_texture == 0)
		return;
}

CTerrain::CTerrain(IDirect3DDevice9 *device,
	std::string heightmapFileName, 
	int cellSpacing, float heightScale)
	:m_device(device), m_tex(0), m_texture(0), m_iDetailRepeat(10),
	m_lightImage(0), m_pos(0, 0, 0), m_isWireFrame(false)
{
	//��ȡReadRawFile
	if (!ReadRawFile(heightmapFileName))
		return;

	m_cellSpacing = cellSpacing;
	m_heightScale = heightScale;


	m_iSize = sqrt(m_heightMap.size()) ;

	m_numVertsPerCol = m_iSize;
	m_numVertsPerRow = m_iSize;

	m_numCellsPerCol = m_numVertsPerCol - 1;
	m_numCellsPerRow = m_numVertsPerRow - 1;

	//����
	m_width = m_numCellsPerRow * m_cellSpacing;
	m_height = m_numCellsPerCol * m_cellSpacing;

	//�������
	m_numVertices = m_numVertsPerRow * m_numVertsPerCol;

	//�����εĸ���
	m_numTriangles = m_numCellsPerCol * m_numCellsPerRow * 2;

	//�Ը߶Ƚ���scale
	for (int i = 0; i < m_heightMap.size(); i++)
		m_heightMap[i] *= m_heightScale;

	if (!ComputeVertices())
		return;

	if (!ComputeIndices())
		return;
	m_texture = new CImage();
	if (m_texture == 0)
		return;

	m_lightImage = new CImage();
}


CTerrain::~CTerrain()
{
	Shutdown();
}

void CTerrain::Shutdown()
{
	m_vb->Release();
	m_ib->Release();
	if (m_tex)
		m_tex->Release();
	if (m_texture)
		delete m_texture;

}


//���ظ߶�ͼ�ĸ߶�
int CTerrain::GetHeightMapEntry(int row, int col)
{
	return m_heightMap[row * m_numVertsPerRow + col];
}

void CTerrain::SetHeightMapEntry(int row, int col, int value)
{
	m_heightMap[row * m_numVertsPerRow + col] = value;
}

float CTerrain::GetHeight(float x, float z)
{
	//���ȶ�x��y����ƽ������
// 	x = ((float)m_width / 2.0f) + x;
// 	z = ((float)m_depth / 2.0f) - z;

	x /= (float)m_cellSpacing;
	z /= (float)m_cellSpacing;


	//���xz���ڵ�cell

	int col = floor(x);
	int row = floor(z);

	if (col < 0 || col >= m_numCellsPerCol || row < 0 || row >= m_numCellsPerRow)
		return 0;

	//��ȡ�����ĸ�����ĸ߶�ֵ
	float A = GetHeightMapEntry(row, col);
	float B = GetHeightMapEntry(row, col + 1);
	float C = GetHeightMapEntry(row + 1, col);
	float D = GetHeightMapEntry(row + 1, col + 1);


	//���delta

	float dx = x - col;
	float dz = z - row;

	float height = 0;
	//�ж����������λ�����������
	if (dz < 1.0f - dx)
	{
		float uy = B - A;
		float vy = C - A;
		height = A + Math::Lerp(0.0f, uy, dx) +
			Math::Lerp(0.0f, vy, dz);
	}
	else
	{
		float uy = C - D;
		float vy = B - D;
		height = D + Math::Lerp(0.0f, uy, 1.0f - dx) +
			Math::Lerp(0.0f, vy, 1.0f - dz);
	}
	return height;
}

//��������
bool CTerrain::LoadTexture(std::string fileName)
{
	HRESULT hr = 0;

	hr = D3DXCreateTextureFromFile(m_device, Convert::str2Wstr(fileName).c_str(), &m_tex);
	if (FAILED(hr))
		return false;

	return true;
}


//Ϊÿ�����������������꣬��һ�����������ڵ�ͼ��
bool CTerrain::GenerateTexture(D3DXVECTOR3 *directionLight)
{

	return true;
}

bool CTerrain::Render()
{
	HRESULT hr = 0;

	if (m_device)
	{
		D3DXMATRIX world;
		D3DXMatrixTranslation(&world, m_pos.x, m_pos.y, m_pos.z);

		m_device->SetTransform(D3DTS_WORLD, &world);

		//��������Դ
		m_device->SetStreamSource(0, m_vb, 0, sizeof(TerrainVertex));
		m_device->SetFVF(TerrainVertex::FVF);
		m_device->SetIndices(m_ib);


		SetRenderTexture();


		//�رչ���
		m_device->SetRenderState(D3DRS_LIGHTING, false);

		if (m_isWireFrame)
		{
			//�߿�ģʽ
			m_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

			hr = m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
				0, 0,
				m_numVertices,
				0, m_numTriangles);

			
			m_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}
		else
		{
			//ʵ������ģʽ
			hr = m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
				0, 0,
				m_numVertices, 0, m_numTriangles);
		}

		//�ָ�����
		m_device->SetRenderState(D3DRS_LIGHTING, true);


		if (FAILED(hr))
			return false;
	}
	return true;
}


//��ȡ�߶�ͼ
bool CTerrain::ReadRawFile(std::string fileName)
{
	
	std::ifstream inFile(fileName, std::ios_base::binary);

	if (!inFile.good())
		return false;

	//��ȡ�ļ�����
	inFile.seekg(0, inFile.end);
	int length = inFile.tellg();
	inFile.seekg(0, inFile.beg);

	std::vector<BYTE> in(length);

	//��ȡ����
	inFile.read((char *)&in[0], in.size());

	m_heightMap.resize(length);

	int temSize = sqrt(length);

	for (int i = 0; i < in.size(); i++)
	{
		m_heightMap[i] = in[i];
	}

	for (int i = 0; i < temSize; i++)
	{
		m_heightMap[(temSize - 1) * temSize + i] = m_heightMap[i];
	}
	for (int i = 0; i < temSize; i++)
	{
		m_heightMap[i * temSize + temSize - 1] = m_heightMap[i * temSize];
	}

	m_heightMap[temSize * temSize - 1] = m_heightMap[0];

	inFile.close();

// 	int size = 257;
// 	m_heightMap.resize(size * size);
// 
// 	for (int i = 0; i < size; i++)
// 	{
// 		for (int j = 0; j < size; j++)
// 		{
// 			m_heightMap[i * size + j] = rand() % 30;
// 		}
// 	}
	return true;
}

bool CTerrain::ComputeVertices()
{
	HRESULT hr;

	hr = m_device->CreateVertexBuffer(m_numVertices * sizeof(TerrainVertex),
		D3DUSAGE_WRITEONLY,
		TerrainVertex::FVF,
		D3DPOOL_MANAGED,
		&m_vb,
		0);

	if (FAILED(hr))
		return false;

	int startX = -m_width / 2;
	int startZ = m_height / 2;

	int endX = m_width / 2;
	int endZ = -m_height / 2;


	float uCoordTex = 1.0f / (float)m_numCellsPerRow;
	float vCoordTex = 1.0f / (float)m_numCellsPerCol;

	TerrainVertex *tv = 0;
	m_vb->Lock(0, 0, (void **)&tv, 0);

	int i = 0;
	int scale = 1;
	for (int z = 0; z <= m_height; z+= m_cellSpacing)
	{
		int j = 0;

		for (int x = 0; x <= m_width; x += m_cellSpacing)
		{
			int index = i * m_numVertsPerRow + j;
			tv[index] = TerrainVertex((float)(x * scale), (float)m_heightMap[index], (float)(z * scale),
				(float)i * uCoordTex, (float)j * vCoordTex);

			tv[index].du = z * (1.0f / (float)m_iSize) * (float)m_iDetailRepeat;
			tv[index].dv = x * (1.0f / (float)m_iSize) * (float)m_iDetailRepeat;
			m_vertices.push_back(tv[index] *scale);
			j++;
		}
		i++;
	}
	m_vb->Unlock();
	return true;
}

bool CTerrain::ComputeIndices()
{
	HRESULT hr = 0;

	//16λ������
	hr = m_device->CreateIndexBuffer(m_numTriangles * 3 * sizeof(DWORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_ib,
		0);

	if (FAILED(hr))
		return false;

	DWORD *indices = 0;

	int baseIndex = 0;
	m_ib->Lock(0, 0, (void **)&indices, 0);

	for (int i = 0; i < m_numCellsPerCol; i++)
	{
		for (int j = 0; j < m_numCellsPerRow; j++)
		{
			indices[baseIndex] = i * m_numVertsPerRow + j;
			indices[baseIndex + 1] = i * m_numVertsPerRow + j + 1;
			indices[baseIndex + 2] = (i + 1) * m_numVertsPerRow + j;

			indices[baseIndex + 3] = (i + 1) * m_numVertsPerRow + j;
			indices[baseIndex + 4] = i * m_numVertsPerRow + j + 1;
			indices[baseIndex + 5] = (i + 1) * m_numVertsPerRow + j + 1;

			
			baseIndex += 6;
		}
	}
	m_ib->Unlock();
	return true;
}



bool CTerrain::LightTerrain(D3DXVECTOR3 *directionLight)
{
	return true;
}

float CTerrain::ComputeShade(int cellRow, int cellCol, D3DXVECTOR3 *directionLight)
{
	return 0;
}



//uiSize,С����Ĵ�С
void CTerrain::GenerateTextureMap(unsigned int uiSize)
{
	float fTotalRed, fTotalGreen, fTotalBlue;
	unsigned char ucRed, ucGreen, ucBlue;

	float fMapRatio = 0.0f;
	int iLastHeight = 0;

	unsigned int uiTexX, uiTexZ;

	float fBlend[NUM_TILES];

	//��ʼ�������ש������
	m_tiles.iNumTiles = 0;

	for (int i = 0; i < NUM_TILES; i++)
	{
		if (m_tiles.m_textureTiles[i].isLoad())
			m_tiles.iNumTiles++;
	}

	iLastHeight = 0;

	//����ÿ�������Ӱ�췶Χ
	for (int i = 0; i < NUM_TILES; i++)
	{
		//�����Ѿ�����
		if (m_tiles.m_textureTiles[i].isLoad())
		{
			m_tiles.m_regions[i].m_iLowHeight = iLastHeight;

			//�Ը߶�ͼ�ȼ�����ƽ��
			iLastHeight += 255 / (m_tiles.iNumTiles + 1);

			m_tiles.m_regions[i].m_iOptimalHeight = iLastHeight;

			m_tiles.m_regions[i].m_iHighHeight = (iLastHeight - m_tiles.m_regions[i].m_iLowHeight) + iLastHeight;
		}
	}

	//��������,uiSize * uiSize 24λ
	m_texture->create(uiSize, uiSize, 24);

	fMapRatio = (float)m_iSize / (float)uiSize;

	//���������ϵ�ÿ�����ص���ɫ
	for (int z = 0; z < uiSize; z++)
	{
		for (int x = 0; x < uiSize; x++)
		{
			fTotalRed = 0.0f;
			fTotalGreen = 0.0f;
			fTotalBlue = 0.0f;

			for (int i = 0; i < NUM_TILES; i++)
			{
				if (m_tiles.m_textureTiles[i].isLoad())
				{
					uiTexX = x;
					uiTexZ = z;

					//���ݵ��ε�x��z����ȡ�����λ��
					GetTexCoords(&m_tiles.m_textureTiles[i], &uiTexX, &uiTexZ);

					//��ȡ�����Ӧ����ɫֵ
					m_tiles.m_textureTiles[i].getColor(uiTexX, uiTexZ, &ucRed, &ucGreen, &ucBlue);


					//���㵱ǰtile�İٷֱ�
					fBlend[i] = RegionPercent(i, InterpolateHeight(x, z, fMapRatio));

					fTotalRed += fBlend[i] * ucRed;
					fTotalGreen += fBlend[i] * ucGreen;
					fTotalBlue += fBlend[i] * ucBlue;
				}
			}

			//���õ����ܵ���ɫ����
			m_texture->setColor(x, z, (unsigned char)(fTotalRed),
				(unsigned char)(fTotalGreen ),
				(unsigned char)(fTotalBlue));
		}
	}

	ImageToTexture(m_texture, &m_tex);
}

void CTerrain::GetTexCoords(CImage *texture, unsigned int *x, unsigned int *z)
{
	unsigned int uiWidth = texture->getWidth();
	unsigned int uiHeight = texture->getHeight();

	int repeatX = -1;
	int repeatZ = -1;

	int index = 0;

	while (repeatX == -1)
	{
		index++;
		if (*x < (index * uiWidth))
			repeatX = index - 1;
	}

	index = 0;

	while (repeatZ == -1)
	{
		index++;
		if (*z < (index * uiHeight))
			repeatZ = index - 1;
	}
	
	*x = *x - (repeatX * uiWidth);
	*z = *z - (repeatZ * uiHeight);
}


//������㵱ǰ�߶���ĳ��tile�İٷֱ�
float CTerrain::RegionPercent(int tileType, unsigned char ucHeight)
{
	float fTemp1, fTemp2;

	if (m_tiles.m_textureTiles[LOWEST_TILE].isLoad())
	{
		if (tileType == LOWEST_TILE && ucHeight < m_tiles.m_regions[LOWEST_TILE].m_iOptimalHeight)
			return 1.0f;
	}
	else if (m_tiles.m_textureTiles[LOW_TILE].isLoad())
	{
		if (tileType == LOW_TILE && ucHeight < m_tiles.m_regions[LOW_TILE].m_iOptimalHeight)
			return 1.0f;
	}
	else if (m_tiles.m_textureTiles[HIGH_TILE].isLoad())
	{
		if (tileType == HIGH_TILE && ucHeight < m_tiles.m_regions[HIGH_TILE].m_iOptimalHeight)
			return 1.0f;
	}
	else if (m_tiles.m_textureTiles[HIGHEST_TILE].isLoad())
	{
		if (tileType == HIGHEST_TILE && ucHeight < m_tiles.m_regions[HIGHEST_TILE].m_iOptimalHeight)
			return 1.0f;
	}

	if (ucHeight < m_tiles.m_regions[tileType].m_iLowHeight || ucHeight > m_tiles.m_regions[tileType].m_iHighHeight)
		return 0.0f;

	if (ucHeight < m_tiles.m_regions[tileType].m_iOptimalHeight)
	{
		fTemp1 = ucHeight - m_tiles.m_regions[tileType].m_iLowHeight;
		fTemp2 = m_tiles.m_regions[tileType].m_iOptimalHeight - m_tiles.m_regions[tileType].m_iLowHeight;

		return (fTemp1 / fTemp2);
	}
	else if (ucHeight == m_tiles.m_regions[tileType].m_iOptimalHeight)
		return 1.0f;
	else if (ucHeight > m_tiles.m_regions[tileType].m_iOptimalHeight)
	{
		fTemp1 = m_tiles.m_regions[tileType].m_iHighHeight - ucHeight;
		fTemp2 = m_tiles.m_regions[tileType].m_iHighHeight - m_tiles.m_regions[tileType].m_iOptimalHeight;
		return (fTemp1 / fTemp2);
	}
	return 0.0f;
}


//���������λ�ã��������Ӧ��heightMap�ĸ߶�
unsigned char CTerrain::InterpolateHeight(int x, int z, float fHeightToTexRatio)
{
	unsigned char ucLow, ucHighx, ucHighz;
	
	float fScaledX = x * fHeightToTexRatio;
	float fScaledZ = z * fHeightToTexRatio;
	
	return GetHeight(fScaledX, fScaledZ) / m_heightScale;
}

bool CTerrain::LoadTile(ETILE_TYPE tileType, std::string fileName)
{
	return m_tiles.m_textureTiles[tileType].loadData(fileName);
}

void CTerrain::UnloadTile(ETILE_TYPE tileType)
{
	m_tiles.m_textureTiles[tileType].unload();
}

void CTerrain::UnloadAllTiles()
{
	UnloadTile(LOW_TILE);
	UnloadTile(LOWEST_TILE);
	UnloadTile(HIGH_TILE);
	UnloadTile(HIGHEST_TILE);
}

bool CTerrain::SaveTextureMap(std::string szFilename)
{
	if (m_texture->isLoad())
		return m_texture->save(szFilename);
	return false;
}

bool CTerrain::ImageToTexture(CImage *image, IDirect3DTexture9 **m_texture )
{
	if (image == 0)
		return false;

	HRESULT hr = D3DXCreateTexture(m_device, image->getWidth(), image->getHeight(),
		D3DX_DEFAULT, 0, D3DFMT_R8G8B8, D3DPOOL_MANAGED, &(*m_texture));
	if (FAILED(hr))
		return false;

	D3DLOCKED_RECT d3d_lRect;

	if (FAILED((*m_texture)->LockRect(0, &d3d_lRect, 0, 0)))
		return false;

	unsigned char *pDest = (unsigned char *)d3d_lRect.pBits;

	unsigned char *pSrc = image->getData();

	for (int i = 0; i < image->getHeight(); i++)
	{
		for (int j = 0; j < image->getWidth(); j++)
		{
			for (int k = 0; k < 3; k++)
			{
				*(pDest + k) = *(pSrc + k);
			}
			pDest += 4;
			pSrc += 3;
		}
	}
	(*m_texture)->UnlockRect(0);
}

bool CTerrain::LoadDetailTexture(std::string fileName)
{
	if (FAILED(D3DXCreateTextureFromFile(m_device, Convert::str2Wstr(fileName).c_str(), &m_detailTex)))
		return false;

	return true;
}

void CTerrain::ComputeDetailTextureCoord()
{
	
}

void CTerrain::SetRenderTexture()
{

	m_device->SetTexture(0, m_tex);
	m_device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	m_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);


// 	m_device->SetTexture(1, m_lightTexture);
// 	m_device->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
// 	m_device->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
// 	m_device->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
// 	m_device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE2X);

	m_device->SetTexture(1, m_detailTex);
	m_device->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
	m_device->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_device->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	m_device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
}

void CTerrain::CalculateLighting()
{
	float fShade = 0.0f;

	if (m_lightmap.m_iSize != m_iSize || m_lightmap.m_ucpData == 0)
	{
		delete[]m_lightmap.m_ucpData;

		//���·����ڴ�
		m_lightmap.m_ucpData = new unsigned char[m_iSize * m_iSize];
		if (m_lightmap.m_ucpData == 0)
			return;
		m_lightmap.m_iSize = m_iSize;
	}

	//�������ÿ�����������ɫֵ
	for (int z = 0; z < m_iSize; z ++)
	{
		for (int x = 0; x < m_iSize; x++)
		{
			float sd = GetHeight(x - m_iDirectionX, z - m_iDirectionZ);
			float df = GetHeight(x, z);
			if (z >= m_iDirectionZ && x >= m_iDirectionX)
				fShade = 1.0f - (GetHeight(x - m_iDirectionX, z - m_iDirectionZ) -
				GetHeight(x, z)) / m_fLightSoftness;

			else
				fShade = 0.0f;

			if (fShade < m_fMinBrightness)
				fShade = m_fMinBrightness;
			if (fShade > m_fMaxBrightness)
				fShade = m_fMaxBrightness;

			SetBrightnessAtPoint(x, z, fShade * 255);
		}
		
	}



	std::ifstream inFile("media/lightmap.raw", std::ios_base::binary);


	//��ȡ�ļ�����
	inFile.seekg(0, inFile.end);
	int length = inFile.tellg();
	inFile.seekg(0, inFile.beg);

	std::vector<BYTE> in(length);

	//��ȡ����
	inFile.read((char *)&in[0], in.size());

// 	for (int i = 0; i < in.size(); i++)
// 	{
// 		m_lightmap.m_ucpData[i] = in[i];
// 	}
	
	inFile.close();

	GetLightTexture();
}



void CTerrain::GetLightTexture()
{
	if (m_lightmap.m_ucpData == 0)
		return ;

	HRESULT hr = D3DXCreateTexture(m_device, m_lightmap.m_iSize, m_lightmap.m_iSize,
		D3DX_DEFAULT, 0, D3DFMT_R8G8B8, D3DPOOL_MANAGED, &m_lightTexture);
	if (FAILED(hr))
		return ;

	D3DLOCKED_RECT d3d_lRect;

	if (FAILED(m_lightTexture->LockRect(0, &d3d_lRect, 0, 0)))
		return ;

	unsigned char *pDest = (unsigned char *)d3d_lRect.pBits;

	unsigned char *pSrc = m_lightmap.m_ucpData;


	for (int i = 0; i < m_lightmap.m_iSize; i++)
	{
		for (int j = 0; j < m_lightmap.m_iSize; j++)
		{
			unsigned char d = *pSrc;
			//*pSrc = rand() % 50 + 200;
			*pDest = *pSrc * m_vecLightColor.x;
			*(pDest + 1) = *pSrc * m_vecLightColor.y;
			*(pDest + 2) = *pSrc * m_vecLightColor.z;
			pDest += 4;
			pSrc++;
		}
	}
	m_lightTexture->UnlockRect(0);
}




