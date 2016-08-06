#ifndef __SNOW_PARTICLE_H__
#define __SNOW_PARTICLE_H__

#include "particleSystem.h"
#include <vector>
#include "ParticleConfig.h"
#include "EmitterConfig.h"

class SnowParticle : public ParticleSystem
{
public:
	SnowParticle(IDirect3DDevice9 *device, const ParticleConfig *particleConfig,
		const EmitterConfig *eConfig);
	~SnowParticle();

	virtual bool Init();
	virtual void Update(float elapsed);
	virtual void Render();
	virtual bool AddParticle();
	virtual void ResetParticle(Particle* particle);

private:
	IDirect3DDevice9 *m_device;
	IDirect3DVertexBuffer9 *m_vb;
	std::vector<IDirect3DTexture9 *> m_snowTextures;

	int m_numSnow;

	std::vector<Particle> m_snowPartices;
	int m_snowSize;  //—©ª®¥Û–°

	std::vector<std::string>m_textureNames;
};
#endif