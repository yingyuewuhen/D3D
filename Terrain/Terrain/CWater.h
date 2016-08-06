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

	void Draw(float gameTick);  //���ƺ���
	void Release();  //�ͷ���Դ
	void BeginReflect();
	void EndReflect();
	void BeginRefract();
	void EndRefract();

private:
	bool LoadContent();  //������Դ
	void CreateWaterVertices();  //����ˮ�涥������
	void FetchSurfaces();  //��ȡ�����������
	void GetParamHandles();  //��ȡĬ�ϲ������
	void SetDefaultParamValues();  //����Ĭ�ϵľ������
	void ResetParamValues();  //�������в���

private:
	D3DXHANDLE m_hWorldMatrix;  //�������
	D3DXHANDLE m_hViewMatrix;  //��Ӱ����
	D3DXHANDLE m_hProjMatrix;
	D3DXHANDLE m_hCameraPos;
	D3DXHANDLE m_hReflectMatrix;

	D3DXHANDLE m_hWaveHeight;  //ˮ�����
	D3DXHANDLE m_hWindForce;  //����

	D3DXHANDLE m_hWindDirection;
	D3DXHANDLE m_hWaveTexture;
	D3DXHANDLE m_hWaveTextureUVTile;  //����ƽ�̴���
	D3DXHANDLE m_hReflectTexture;  //��������
	D3DXHANDLE m_hRefractTexture;   //��������

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

	IDirect3DVertexBuffer9 *m_pVB;  //���㻺����

	D3DXPLANE m_waterPlane;  //ˮƽ��
	D3DXPLANE m_refWaterPlane;  //����ˮƽ��
	D3DXMATRIX m_worldMatrix;


	Camera *m_camera;
private:


};
#endif