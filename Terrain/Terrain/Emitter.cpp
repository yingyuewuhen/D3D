#include "Emitter.h"
#include "resourse.h"
#include "Convert.h"
#include <vector>


Emitter::Emitter(IDirect3DDevice9 *device,
	const EmitterConfig *emitterConfig,
	const ParticleConfig *particleConfig)
	:m_device(device), m_particleTex(0), m_vb(0)
{
	m_maxNumPaticles = emitterConfig->maxNumParticles;
	m_numParticleToAdd = emitterConfig->numParticleToAdd;
	m_pos = emitterConfig->position;

	m_vbSize = m_maxNumPaticles;


	m_vbOffset = 0;
	m_vbBatchSize = 500;

	m_particleColor = particleConfig->color;
	m_particleTexNames = particleConfig->textureNames;
	m_particleLifeTime = particleConfig->lifeTime;
}

Emitter::~Emitter()
{
	

}

bool Emitter::Init()
{
	//创建粒子顶点
	if (FAILED(m_device->CreateVertexBuffer(m_vbSize * sizeof(POINTVERTEX),
		D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS,
		D3DFVF_POINTVERTEX, D3DPOOL_MANAGED, &m_vb, 0)))
		return false;

	std::string resoursePath = Resourse::GetResoursePath();

	IDirect3DTexture9 *tex = 0;
	for (std::vector<std::string>::iterator it = m_particleTexNames.begin();
		it != m_particleTexNames.end();
		it++)
	{
		if (FAILED(D3DXCreateTextureFromFile(m_device, Convert::str2Wstr(resoursePath + *it).c_str(),
			&tex)))
			return false;
		m_particleTex.push_back(tex);
	}
	return true;
}

void Emitter::Update(float elapsed)
{
	for (std::vector<Particle>::iterator it = m_particles.begin();
		it != m_particles.end(); 
		it++)
	{
		if (it->isLive)  //目前存活
		{
			it->position += it->velocity * 0.1f;
			it->age += 0.2f;
			it->color = this->m_particleColor;
			if (it->age > it->lifeTime)
				it->isLive = false;
		}
		else
			ResetParticle(static_cast<Particle *>(&(*it)));
	}
	for (int i = 0; i < m_numParticleToAdd && m_particles.size() < m_vbSize; i++)
	{
		AddParticle();
	}
}

void Emitter::Render()
{
	if (m_device)
	{
		m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);  //开启alpha混合
		m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		m_device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
		m_device->SetRenderState(D3DRS_POINTSCALEENABLE, true);

		//设置
		m_device->SetRenderState(D3DRS_POINTSIZE, FloatToDword(0.50f));
		m_device->SetRenderState(D3DRS_POINTSIZE_MIN, FloatToDword(0.00f));
		m_device->SetRenderState(D3DRS_POINTSCALE_A, FloatToDword(0.00f));
		m_device->SetRenderState(D3DRS_POINTSCALE_B, FloatToDword(0.00f));
		m_device->SetRenderState(D3DRS_POINTSCALE_C, FloatToDword(1.00f));

		//设置纹理
		m_device->SetTexture(0, m_particleTex[0]);
		m_device->SetStreamSource(0, m_vb, 0, sizeof(POINTVERTEX));
		m_device->SetFVF(D3DFVF_POINTVERTEX);

		if (m_vbOffset > m_vbSize)
			m_vbOffset = 0;

		POINTVERTEX *vertex = 0;
		m_vb->Lock(m_vbOffset * sizeof(POINTVERTEX),
			m_vbBatchSize * sizeof(POINTVERTEX),
			(void **)&vertex, 
			m_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticleinBatch = 0;

		for (std::vector<Particle>::iterator it = m_particles.begin();
			it != m_particles.end(); 
			it++)
		{
			if (it->isLive)
			{
				vertex->pos = it->position;
				vertex->color = it->color;

				vertex++;
				numParticleinBatch++;
				if (numParticleinBatch == m_vbBatchSize)
				{
					m_vb->Unlock();
					m_device->DrawPrimitive(D3DPT_POINTLIST, m_vbOffset, m_vbBatchSize);
					m_vbOffset += m_vbBatchSize;

					if (m_vbOffset > m_vbSize)
						m_vbOffset = 0;

					m_vb->Lock(
						m_vbOffset * sizeof(POINTVERTEX),
						m_vbBatchSize * sizeof(POINTVERTEX),
						(void**)&vertex,
						m_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD
						);

					numParticleinBatch = 0;
				}
			}
		}
		m_vb->Unlock();

		if (numParticleinBatch)
		{
			m_device->DrawPrimitive(
				D3DPT_POINTLIST,
				m_vbOffset,
				numParticleinBatch
				);
		}

		m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}
}

bool Emitter::AddParticle()
{
	Particle particle;
	
	//设置particle的各种参数
	ResetParticle(&particle);

	m_particles.push_back(particle);
	return true;
}

void Emitter::ResetParticle(Particle* particle)
{
	particle->isLive = true;
	particle->age = 0.0f;
	if (particle->lifeTime != -1)
	{
		particle->lifeTime = m_particleLifeTime;
	}
	else
	{
		particle->lifeTime = GetRandomFloat(0, 1);
	}
	particle->position = m_pos;
	particle->color = m_particleColor;
	
	D3DXVECTOR3 maxVelocity(1, 1, 1);
	D3DXVECTOR3 minVelocity(-1, -1, -1);

	D3DXVECTOR3 velocity;
	GetRandomVector(&velocity, &minVelocity, &maxVelocity);
	D3DXVec3Normalize(&velocity, &velocity);
	particle->velocity = velocity;
}
