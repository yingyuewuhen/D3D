#ifndef __EMITTER_H__
#define __EMITTER_H__

#include "particleSystem.h"
#include "EmitterConfig.h"
#include "ParticleConfig.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <vector>
class Emitter : public ParticleSystem
{
public:
	Emitter(IDirect3DDevice9 *device, 
		const EmitterConfig *emitterConfig,
		const ParticleConfig *particleConfig);
	virtual ~Emitter();

	virtual bool Init();
	virtual void Update(float elapsed);
	virtual void Render();
	virtual bool AddParticle();
	virtual void ResetParticle(Particle* particle);

	
private:
	IDirect3DDevice9 *m_device;
	IDirect3DVertexBuffer9 *m_vb;
	std::vector<IDirect3DTexture9 *>m_particleTex;

	//粒子的位置
	D3DXVECTOR3 m_pos;
	int m_numParticleToAdd;
	int m_maxNumPaticles;
	int m_vbSize;  //顶点缓冲区的大小
	int m_vbOffset;
	int m_vbBatchSize;

	DWORD m_particleColor;
	std::vector<std::string> m_particleTexNames;
	int m_particleLifeTime;

	std::vector<Particle>m_particles;
};
#endif