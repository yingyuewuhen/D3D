#include "CWater.h"


CWater::CWater(IDirect3DDevice9 *device, Camera *camera)
	:m_device(device), m_camera(camera)
{
	m_pWaterEffect = new D3DEffect(device);
}

CWater::~CWater()
{

}


//创建水面
bool CWater::Create(int iWidth, int iHeight, 
	float fPosX, float fPosY, float fHeight)
{
	//加载纹理
	if (!LoadContent())
		return false;

	//获取反射和折射纹理，并获取表面
	FetchSurfaces();

	//创建顶点缓冲区
	HRESULT hr = m_device->CreateVertexBuffer(6 * sizeof(VertexPositionTex),
		D3DUSAGE_WRITEONLY, VertexPositionTex::FVF, D3DPOOL_MANAGED,
		&m_pVB, 0);

	if (FAILED(hr))
		return false;

	VertexPositionTex *vertex = 0;
	//锁住缓存
	m_pVB->Lock(0, 0, (void **)&vertex, 0);
	vertex[0] = VertexPositionTex(fPosX, fHeight, fPosY + iHeight, 0, 0);
	vertex[1] = VertexPositionTex(fPosX + iWidth, fHeight, fPosY + iHeight, 1, 0);
	vertex[2] = VertexPositionTex(fPosX, fHeight, fPosY, 0, 1);

	vertex[3] = VertexPositionTex(fPosX + iWidth, fHeight, fPosY + iHeight, 1, 0);
	vertex[4] = VertexPositionTex(fPosX + iWidth, fHeight, fPosY, 1, 1);
	vertex[5] = VertexPositionTex(fPosX, fHeight, fPosY, 0, 1);
	m_pVB->Unlock();


	//创建折射和反射横切面，相当于水面

	D3DXPlaneFromPointNormal(&m_waterPlane, &D3DXVECTOR3(fPosX, fHeight, fPosY), &D3DXVECTOR3(0, 1.0f, 0));
	//归一化
	D3DXPlaneNormalize(&m_waterPlane, &m_waterPlane);

	m_camera->SetReflectPlane(m_waterPlane);

	return true;
}

void CWater::Draw(float gameTick)
{
	// 重置特效参数
	ResetParamValues();

	m_pWaterEffect->GetEffect()->SetFloat(m_hTimeTick, 0.001);

	//应用特效
	UINT numPasses;
	HRESULT hr = m_pWaterEffect->BeginEffect(numPasses);
	
	for (int i = 0; i < numPasses; i++)
	{
		hr = m_pWaterEffect->GetEffect()->BeginPass(i);

		if (hr == D3D_OK)
			hr++;

		//绘制顶点
		m_device->SetStreamSource(0, m_pVB, 0, sizeof(VertexPositionTex));
		m_device->SetFVF(VertexPositionTex::FVF);
		m_device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		m_pWaterEffect->GetEffect()->EndPass();
	}
	m_pWaterEffect->EndEffect();
}

void CWater::Release()
{

}


//开始渲染反射面
void CWater::BeginReflect()
{
	m_device->SetTransform(D3DTS_VIEW, &m_camera->getReflectMatrix());

	//设置横切面，仅反射水面以上的物体
	m_device->SetClipPlane(0, m_waterPlane);
	m_device->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);  //设置裁减面

	//保留原始的渲染目标
	m_device->GetRenderTarget(0, &m_pOriginSurface);

	//设定新的渲染目标
	m_device->SetRenderTarget(0, m_pReflectSurface);

	//清空反射表面缓存
	m_device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(100, 149, 237, 255), 1.0f, 0);
}

void CWater::EndReflect()
{
	//还原渲染目标
	m_device->SetRenderTarget(0, m_pOriginSurface);
	
	//禁用横切面
	m_device->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);

	//还原摄影矩阵
	m_device->SetTransform(D3DTS_VIEW, &m_camera->getViewMatrix());

	D3DXSaveSurfaceToFile(L"paper.jpg", D3DXIFF_JPG, m_pReflectSurface, NULL, NULL);
}


//开始折射渲染
void CWater::BeginRefract()
{
	//设置折射面
	m_device->SetClipPlane(0, -m_waterPlane);
	m_device->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);

	//保留原始目标
	m_device->GetRenderTarget(0, &m_pOriginSurface);

	//设置新的目标
	m_device->SetRenderTarget(0, m_pRefractSurface);

	//清空缓存
	m_device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(100, 149, 237, 255), 1.0f, 0);
}

void CWater::EndRefract()
{
	//还原目标
	m_device->SetRenderTarget(0, m_pOriginSurface);

	//禁用横切面
	m_device->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
}


//记载资源文件
bool CWater::LoadContent()
{
	if (!m_pWaterEffect->LoadEffect("effect/Water.fx"))
		return false;


	//加载纹理
	HRESULT hr = 0;
	hr = D3DXCreateTextureFromFile(m_device, L"media/WaterWav.jpg", &m_pWaterWaveTexture);
	if (FAILED(hr))
		return false;

	GetParamHandles();
	SetDefaultParamValues();


	return true;
}

void CWater::CreateWaterVertices()
{

}


//获取表面
void CWater::FetchSurfaces()
{
	//创建反射纹理并获得其渲染表面
	D3DXCreateTexture(m_device, 800, 450, 1, D3DUSAGE_RENDERTARGET,
		D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &m_pReflectTexture);

	//获得反射表面
	m_pReflectTexture->GetSurfaceLevel(0, &m_pReflectSurface);

	//创建折射纹理
	D3DXCreateTexture(m_device, 800, 450, 1, D3DUSAGE_RENDERTARGET,
		D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &m_pRefractTexture);
	m_pRefractTexture->GetSurfaceLevel(0, &m_pRefractSurface);
}

void CWater::GetParamHandles()
{
	m_hWorldMatrix = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gWorld");
	m_hViewMatrix = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gView");
	m_hProjMatrix = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gProjection");
	m_hCameraPos = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gCameraPos");
	m_hReflectMatrix = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gReflectionView");
	m_hWaveHeight = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gWaveHeight");
	m_hWindForce = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gWindForce");
	m_hWindDirection = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gWindDirection");
	m_hWaveTexture = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gTexture1");
	m_hWaveTextureUVTile = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gTexture1UVTile");
	m_hReflectTexture = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gReflectionMap");
	m_hRefractTexture = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gRefractionMap");
	m_hTimeTick = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gTime");
}

void CWater::SetDefaultParamValues()
{
	D3DXHANDLE waterTechnique = m_pWaterEffect->GetEffect()->GetTechniqueByName("Water");

	m_pWaterEffect->GetEffect()->SetTechnique(waterTechnique);  //设置急速

	m_pWaterEffect->GetEffect()->SetTexture(m_hWaveTexture, m_pWaterWaveTexture);
	m_pWaterEffect->GetEffect()->SetFloat(m_hWaveTextureUVTile, 10.0f);
	m_pWaterEffect->GetEffect()->SetFloat(m_hWaveHeight, 0.3f);
	m_pWaterEffect->GetEffect()->SetFloat(m_hWindForce, 0.2f);
	m_pWaterEffect->GetEffect()->SetVector(m_hWindDirection, &D3DXVECTOR4(1, 0, 1, 1));
}

void CWater::ResetParamValues()
{
	m_device->GetTransform(D3DTS_WORLD, &m_worldMatrix);

	D3DXVECTOR3 camPos = m_camera->getPosition();

	m_pWaterEffect->GetEffect()->SetMatrix(m_hWorldMatrix, &m_worldMatrix); // 设置世界矩阵
	m_pWaterEffect->GetEffect()->SetMatrix(m_hViewMatrix, &m_camera->getViewMatrix());

	m_pWaterEffect->GetEffect()->SetMatrix(m_hProjMatrix, &m_camera->getProjMatrix());
	m_pWaterEffect->GetEffect()->SetVector(m_hCameraPos, &D3DXVECTOR4(camPos.x, camPos.y, camPos.z, 1));
	m_pWaterEffect->GetEffect()->SetMatrix(m_hReflectMatrix, &m_camera->getReflectMatrix());  //设置反射矩阵
	m_pWaterEffect->GetEffect()->SetTexture(m_hReflectTexture, m_pReflectTexture);  //设置反射纹理

	m_pWaterEffect->GetEffect()->SetTexture(m_hRefractTexture, m_pRefractTexture);  //设置折射纹理

}
