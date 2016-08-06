#ifndef __EMITTER_CONFIG_H__
#define __EMITTER_CONFIG_H__
#include <d3dx9.h>

class EmitterConfig
{
public:
	D3DXVECTOR3 position;
	int numParticleToAdd;
	int maxNumParticles;
};
#endif