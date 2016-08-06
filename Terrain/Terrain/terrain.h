#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <string>

#include <vector>

#include <d3d9.h>
#include <d3dx9.h>
#include "CImage.h"

//纹理图的个数
const int NUM_TILES = 4;

enum ETILE_TYPE
{
	LOWEST_TILE = 0,
	LOW_TILE,
	HIGH_TILE,
	HIGHEST_TILE
};


struct TEXTURE_REGIONS
{
	int m_iLowHeight;
	int m_iOptimalHeight;
	int m_iHighHeight;
};

struct TEXTURE_TILES
{
	TEXTURE_REGIONS m_regions[NUM_TILES];
	CImage m_textureTiles[NUM_TILES];
	int iNumTiles;
};


struct STRN_LIGHT_DATA
{
	unsigned char* m_ucpData;	
	int m_iSize;	
};


struct TerrainVertex
{
	TerrainVertex(){}
	TerrainVertex(float _x, float _y, float _z, float _u, float _v, float _du = 0.0f, float _dv = 0.0f)
		:x(_x), y(_y), z(_z), u(_u), v(_v), du(_du), dv(_dv)
	{}
	TerrainVertex(float _x, float _y, float _z)
		:x(_x), y(_y), z(_z), u(0), v(0), du(0), dv(0)
	{}

	TerrainVertex operator *(int k) const
	{
		return TerrainVertex(x * k, y, z * k, u, v, du, dv);
	}
	float x, y, z;
	float u, v;
	float du, dv;
	static const DWORD FVF;
};


class CTerrain
{
public:

	CTerrain(IDirect3DDevice9 *device, 
		std::string heightmapFileName,
		int numVertsPerRow,
		int numVertsPerCol,
		int cellSpacing,
		float heightScale);

	CTerrain(IDirect3DDevice9 *device,
		std::string heightmapFileName,
		int cellSpacing,
		float heightScale);
	~CTerrain();

	void Shutdown();

	int GetHeightMapEntry(int row, int col);

	void SetHeightMapEntry(int row, int col, int value);

	float GetHeight(float x, float z);

	bool LoadTexture(std::string fileName);
	bool GenerateTexture(D3DXVECTOR3 *directionLight);

	
	virtual bool Render();

	D3DXVECTOR3 &GetPosition()
	{
		return m_pos;
	}

	void SetPosition(const D3DXVECTOR3 &pos)
	{
		m_pos = pos;
	}
	int GetSize()
	{
		return m_iSize;
	}
	//////////////////////////////////////////////////////////////////////////

	void GenerateTextureMap(unsigned int uiSize);

	bool LoadTile(ETILE_TYPE tileType, std::string fileName);


	void UnloadTile(ETILE_TYPE tileType);

	void UnloadAllTiles();


	bool SaveTextureMap(std::string szFilename);


	bool LoadDetailTexture(std::string fileName);


	//计算光照
	void CalculateLighting();

	inline void SlopeLighting(int iDirX, int iDirZ,
		float fMinBrightness, float fMaxBrightness, float fSoftness)
	{
		m_iDirectionX = iDirX;
		m_iDirectionZ = iDirZ;
		m_fMinBrightness = fMinBrightness;
		m_fMaxBrightness = fMaxBrightness;
		m_fLightSoftness = fSoftness;
	}
	inline void SetLightColor(D3DXVECTOR3 lightColor)
	{
		m_vecLightColor = lightColor;
	}

	void SetWireframe(bool isWireFrame)
	{
		m_isWireFrame = isWireFrame;
	}
protected:
	float RegionPercent(int tileType, unsigned char ucHeight);

	void GetTexCoords(CImage *texture, unsigned int *x, unsigned int *z);

	unsigned char InterpolateHeight(int x, int z, float fHeightToTexRatio);

	void SetRenderTexture();

private:
	bool ReadRawFile(std::string fileName);

	bool ComputeVertices();

	bool ComputeIndices();

	bool LightTerrain(D3DXVECTOR3 *directionLight);

	float ComputeShade(int cellRow, int cellCol, D3DXVECTOR3 *directionLight);


	//将CImage 转化为tex接口
	bool ImageToTexture(CImage *image, IDirect3DTexture9 **m_texture);

	void GetLightTexture();

	void ComputeDetailTextureCoord();

	void FilterTerrain();


	//光照函数
	inline void SetBrightnessAtPoint(int x, int z, unsigned char ucBrightness)
	{
		m_lightmap.m_ucpData[z * m_lightmap.m_iSize + x] = ucBrightness;
	}

	inline unsigned char GetBrightnessAtPoint(int x, int z)
	{
		return m_lightmap.m_ucpData[z * m_lightmap.m_iSize + x];
	}

	
	

protected:

	
private:
	
	IDirect3DTexture9 *m_tex;

	

	TEXTURE_TILES m_tiles;
	CImage *m_texture;  //纹理信息
	bool m_bTextureMapping;

	IDirect3DTexture9 *m_detailTex;
protected:

	D3DXVECTOR3 m_pos;

	IDirect3DVertexBuffer9 *m_vb;

	IDirect3DIndexBuffer9 *m_ib;

	int m_iSize;

	bool m_isWireFrame;
	
	std::vector<TerrainVertex> m_vertices;
	std::vector<DWORD> m_indices;

	//属性
	IDirect3DDevice9 *m_device;

	int m_numVertsPerRow;
	int m_numVertsPerCol;

	int m_cellSpacing;
	
	int m_numCellsPerRow;
	int m_numCellsPerCol;

	
	int m_width;
	int m_height;
	int m_numVertices;
	int m_numTriangles;

	float m_heightScale;

	int m_iDetailRepeat;

	std::vector<int> m_heightMap;


	//光照信息
	STRN_LIGHT_DATA m_lightmap;
	D3DXVECTOR3 m_vecLightColor;
	float m_fMinBrightness;
	float m_fMaxBrightness;
	float m_fLightSoftness;
	int m_iDirectionX;
	int m_iDirectionZ;

	IDirect3DTexture9 *m_lightTexture;
	CImage *m_lightImage;

};
#endif