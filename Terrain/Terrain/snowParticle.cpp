#include "SnowParticle.h"
#include "ParticleConfig.h"
#include "resourse.h"
#include "Convert.h"
#include <d3dx9.h>


SnowParticle::SnowParticle(IDirect3DDevice9 *device, 
	const ParticleConfig *particleConfig,
	const EmitterConfig *eConfig)
	:m_device(device)
{
	m_numSnow = eConfig->numParticleToAdd;

	m_snowSize = 2.0f;
	m_textureNames = particleConfig->textureNames;
	
}

SnowParticle::~SnowParticle()
{
	for (std::vector<IDirect3DTexture9 *>::iterator it = m_snowTextures.begin();
		it != m_snowTextures.end();
		it++)
	{
		if (*it)
			(*it)->Release();
	}
	if (m_vb)
		m_vb->Release();


}

bool SnowParticle::Init()
{
	//创建顶点数据
	if (FAILED(m_device->CreateVertexBuffer(m_numSnow * sizeof(POINTVERTEX),
		D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		D3DFVF_POINTVERTEX, D3DPOOL_MANAGED,
		&m_vb, 0)))
		return false;

	//加载纹理
	std::string resoursePath = Resourse::GetResoursePath();

	IDirect3DTexture9 *tex = 0;
	for (std::vector<std::string>::iterator it = m_textureNames.begin();
		it != m_textureNames.end();
		it++)
	{
		if (FAILED(D3DXCreateTextureFromFile(m_device, Convert::str2Wstr(resoursePath + *it).c_str(),
			&tex)))
			return false;
		m_snowTextures.push_back(tex);
	}

	for (int i = 0; i < m_numSnow; i++)
	{
		AddParticle();
	}
	return true;
}


//更新粒子的状态
void SnowParticle::Update(float elapsed)
{
	for (std::vector<Particle>::iterator it = m_snowPartices.begin();
		it != m_snowPartices.end();
		it++)
	{
		if (it->isLive)
		{
			//更新雪花的位置，age
			it->age += 0.1f;
			it->color += it->colorFade;
			it->position += it->velocity * 1.0f;
			if (it->age > it->lifeTime)
				it->isLive = false;
		}
		else
		{
			ResetParticle(static_cast<Particle*>(&(*it)));
		}
	}
}

void SnowParticle::Render()
{
	if (m_device)
	{
		//设置状态
		m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		//设置alpha混合因子
		m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		m_device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
		m_device->SetRenderState(D3DRS_POINTSCALEENABLE, true);

		//m_device->SetRenderState(D3DRS_LIGHTING, false);
		//设置粒子大小
		//设置
		m_device->SetRenderState(D3DRS_POINTSIZE, FloatToDword(1.0f));
		m_device->SetRenderState(D3DRS_POINTSIZE_MIN, FloatToDword(0.00f));
		m_device->SetRenderState(D3DRS_POINTSCALE_A, FloatToDword(0.00f));
		m_device->SetRenderState(D3DRS_POINTSCALE_B, FloatToDword(0.00f));
		m_device->SetRenderState(D3DRS_POINTSCALE_C, FloatToDword(1.00f));

		//设置数据源和纹理
		m_device->SetStreamSource(0, m_vb, 0, sizeof(POINTVERTEX));
		m_device->SetFVF(D3DFVF_POINTVERTEX);
		

		POINTVERTEX *vertex = 0;
		//锁住顶点
		m_vb->Lock(0, 0, (void **)&vertex, 0);

		int numToRender = 0;
		for (std::vector<Particle>::iterator it = m_snowPartices.begin();
			it != m_snowPartices.end(); 
			it++)
		{
			if (it->isLive)
			{
				vertex->pos = it->position;
				vertex->color = it->color;
				numToRender++;
				vertex++;
			}
		}
		m_vb->Unlock();

		int numParticlePerTex = numToRender / m_snowTextures.size();

		for (int i = 0; i < m_snowTextures.size(); i++)
		{
			m_device->SetTexture(0, m_snowTextures[i]);
			m_device->DrawPrimitive(D3DPT_POINTLIST, numParticlePerTex * i, numParticlePerTex);
		}
		int remainToRender = numToRender - numParticlePerTex * m_snowTextures.size();
		if (remainToRender > 0)
			m_device->DrawPrimitive(D3DPT_POINTLIST, 0, remainToRender);

		m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	}
}

bool SnowParticle::AddParticle()
{
	Particle particle;
	ResetParticle(&particle);
	m_snowPartices.push_back(particle);
	return true;
}


//初始化粒子
void SnowParticle::ResetParticle(Particle* particle)
{
	particle->age = 0.0f;
	particle->isLive = true;
	particle->lifeTime = GetRandomFloat(2.0f, 10.0f);
	particle->color = D3DCOLOR_RGBA(rand() % 255, rand() % 255, rand() % 255, rand() % 255);
	particle->colorFade = D3DXCOLOR(D3DCOLOR_RGBA(0, 0, 0, 0));
	
	D3DXVECTOR3 minVec(0, 50, 0);
	D3DXVECTOR3 maxVec(50, 70, 50);
	D3DXVECTOR3 pos;
	GetRandomVector(&pos, &minVec, &maxVec);
	particle->position = pos;
	particle->initVelocity = D3DXVECTOR3(GetRandomFloat(0, 1.0f),
		GetRandomFloat(-1.0f, 0.0f), GetRandomFloat(0.0f, 1.0f));

	particle->velocity = D3DXVECTOR3(GetRandomFloat(-1.0f, 1.0f),
		GetRandomFloat(-1.0f, 0.0f), GetRandomFloat(-1.0f, 1.0f));

}
