#ifndef __CWATER_H__
#define __CWATER_H__

#include <d3dx9.h>
#include <d3d9.h>
#include "CPlantBoard.h"
#include "camera.h"
#include "D3DEffect.h"

class CWater
{
public:
	CWater(IDirect3DDevice9 *device, Camera *camera);
	~CWater();

public:
	bool Create(
		int iWidth,
		int iHeight,
		float fPosX,
		float fPosY,
		float fHeight);

	void Draw(float gameTick);  //绘制河流
	void Release();  //释放资源
	void BeginReflect();
	void EndReflect();
	void BeginRefract();
	void EndRefract();

private:
	bool LoadContent();  //加载资源
	void CreateWaterVertices();  //生成水面顶点数据
	void FetchSurfaces();  //获取反射折射表面
	void GetParamHandles();  //获取默认参数句柄
	void SetDefaultParamValues();  //设置默认的句柄参数
	void ResetParamValues();  //重置所有参数

private:
	D3DXHANDLE m_hWorldMatrix;  //世界矩阵
	D3DXHANDLE m_hViewMatrix;  //摄影矩阵
	D3DXHANDLE m_hProjMatrix;
	D3DXHANDLE m_hCameraPos;
	D3DXHANDLE m_hReflectMatrix;

	D3DXHANDLE m_hWaveHeight;  //水波振幅
	D3DXHANDLE m_hWindForce;  //风力

	D3DXHANDLE m_hWindDirection;
	D3DXHANDLE m_hWaveTexture;
	D3DXHANDLE m_hWaveTextureUVTile;  //纹理平铺次数
	D3DXHANDLE m_hReflectTexture;  //反射纹理
	D3DXHANDLE m_hRefractTexture;   //折射纹理

	D3DXHANDLE m_hTimeTick;
private:       
	
	D3DEffect *m_pWaterEffect;

	IDirect3DDevice9 *m_device;

	IDirect3DTexture9 *m_pWaterWaveTexture;
	IDirect3DTexture9 *m_pReflectTexture;
	IDirect3DTexture9 *m_pRefractTexture;

	IDirect3DSurface9 *m_pReflectSurface;
	IDirect3DSurface9 *m_pRefractSurface;
	IDirect3DSurface9 *m_pOriginSurface;

	IDirect3DVertexBuffer9 *m_pVB;  //顶点缓冲区

	D3DXPLANE m_waterPlane;  //水平面
	D3DXPLANE m_refWaterPlane;  //反向水平面
	D3DXMATRIX m_worldMatrix;


	Camera *m_camera;
private:


};
#endif