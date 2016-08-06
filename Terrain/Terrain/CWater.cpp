#include "CWater.h"


CWater::CWater(IDirect3DDevice9 *device, Camera *camera)
	:m_device(device), m_camera(camera)
{
	m_pWaterEffect = new D3DEffect(device);
}

CWater::~CWater()
{

}


//����ˮ��
bool CWater::Create(int iWidth, int iHeight, 
	float fPosX, float fPosY, float fHeight)
{
	//��������
	if (!LoadContent())
		return false;

	//��ȡ�����������������ȡ����
	FetchSurfaces();

	//�������㻺����
	HRESULT hr = m_device->CreateVertexBuffer(6 * sizeof(VertexPositionTex),
		D3DUSAGE_WRITEONLY, VertexPositionTex::FVF, D3DPOOL_MANAGED,
		&m_pVB, 0);

	if (FAILED(hr))
		return false;

	VertexPositionTex *vertex = 0;
	//��ס����
	m_pVB->Lock(0, 0, (void **)&vertex, 0);
	vertex[0] = VertexPositionTex(fPosX, fHeight, fPosY + iHeight, 0, 0);
	vertex[1] = VertexPositionTex(fPosX + iWidth, fHeight, fPosY + iHeight, 1, 0);
	vertex[2] = VertexPositionTex(fPosX, fHeight, fPosY, 0, 1);

	vertex[3] = VertexPositionTex(fPosX + iWidth, fHeight, fPosY + iHeight, 1, 0);
	vertex[4] = VertexPositionTex(fPosX + iWidth, fHeight, fPosY, 1, 1);
	vertex[5] = VertexPositionTex(fPosX, fHeight, fPosY, 0, 1);
	m_pVB->Unlock();


	//��������ͷ�������棬�൱��ˮ��

	D3DXPlaneFromPointNormal(&m_waterPlane, &D3DXVECTOR3(fPosX, fHeight, fPosY), &D3DXVECTOR3(0, 1.0f, 0));
	//��һ��
	D3DXPlaneNormalize(&m_waterPlane, &m_waterPlane);

	m_camera->SetReflectPlane(m_waterPlane);

	return true;
}

void CWater::Draw(float gameTick)
{
	// ������Ч����
	ResetParamValues();

	m_pWaterEffect->GetEffect()->SetFloat(m_hTimeTick, 0.001);

	//Ӧ����Ч
	UINT numPasses;
	HRESULT hr = m_pWaterEffect->BeginEffect(numPasses);
	
	for (int i = 0; i < numPasses; i++)
	{
		hr = m_pWaterEffect->GetEffect()->BeginPass(i);

		if (hr == D3D_OK)
			hr++;

		//���ƶ���
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


//��ʼ��Ⱦ������
void CWater::BeginReflect()
{
	m_device->SetTransform(D3DTS_VIEW, &m_camera->getReflectMatrix());

	//���ú����棬������ˮ�����ϵ�����
	m_device->SetClipPlane(0, m_waterPlane);
	m_device->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);  //���òü���

	//����ԭʼ����ȾĿ��
	m_device->GetRenderTarget(0, &m_pOriginSurface);

	//�趨�µ���ȾĿ��
	m_device->SetRenderTarget(0, m_pReflectSurface);

	//��շ�����滺��
	m_device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(100, 149, 237, 255), 1.0f, 0);
}

void CWater::EndReflect()
{
	//��ԭ��ȾĿ��
	m_device->SetRenderTarget(0, m_pOriginSurface);
	
	//���ú�����
	m_device->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);

	//��ԭ��Ӱ����
	m_device->SetTransform(D3DTS_VIEW, &m_camera->getViewMatrix());

	D3DXSaveSurfaceToFile(L"paper.jpg", D3DXIFF_JPG, m_pReflectSurface, NULL, NULL);
}


//��ʼ������Ⱦ
void CWater::BeginRefract()
{
	//����������
	m_device->SetClipPlane(0, -m_waterPlane);
	m_device->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);

	//����ԭʼĿ��
	m_device->GetRenderTarget(0, &m_pOriginSurface);

	//�����µ�Ŀ��
	m_device->SetRenderTarget(0, m_pRefractSurface);

	//��ջ���
	m_device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(100, 149, 237, 255), 1.0f, 0);
}

void CWater::EndRefract()
{
	//��ԭĿ��
	m_device->SetRenderTarget(0, m_pOriginSurface);

	//���ú�����
	m_device->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
}


//������Դ�ļ�
bool CWater::LoadContent()
{
	if (!m_pWaterEffect->LoadEffect("effect/Water.fx"))
		return false;


	//��������
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


//��ȡ����
void CWater::FetchSurfaces()
{
	//�������������������Ⱦ����
	D3DXCreateTexture(m_device, 800, 450, 1, D3DUSAGE_RENDERTARGET,
		D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &m_pReflectTexture);

	//��÷������
	m_pReflectTexture->GetSurfaceLevel(0, &m_pReflectSurface);

	//������������
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

	m_pWaterEffect->GetEffect()->SetTechnique(waterTechnique);  //���ü���

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

	m_pWaterEffect->GetEffect()->SetMatrix(m_hWorldMatrix, &m_worldMatrix); // �����������
	m_pWaterEffect->GetEffect()->SetMatrix(m_hViewMatrix, &m_camera->getViewMatrix());

	m_pWaterEffect->GetEffect()->SetMatrix(m_hProjMatrix, &m_camera->getProjMatrix());
	m_pWaterEffect->GetEffect()->SetVector(m_hCameraPos, &D3DXVECTOR4(camPos.x, camPos.y, camPos.z, 1));
	m_pWaterEffect->GetEffect()->SetMatrix(m_hReflectMatrix, &m_camera->getReflectMatrix());  //���÷������
	m_pWaterEffect->GetEffect()->SetTexture(m_hReflectTexture, m_pReflectTexture);  //���÷�������

	m_pWaterEffect->GetEffect()->SetTexture(m_hRefractTexture, m_pRefractTexture);  //������������

}
