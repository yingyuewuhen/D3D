#ifndef __PARTICLE_STSTEM_H__
#define __PARTICLE_STSTEM_H__

#include "particle.h"
#include <d3d9.h>

struct POINTVERTEX
{
	D3DXVECTOR3 pos;
	DWORD color;

};

const DWORD D3DFVF_POINTVERTEX = (D3DFVF_XYZ | D3DFVF_DIFFUSE);


class ParticleSystem
{
public:
	ParticleSystem(){}
	virtual ~ParticleSystem(){}

public:
	virtual bool Init() = 0;
	virtual void Update(float elapsed) = 0;
	virtual void Render() = 0;
	virtual bool AddParticle() = 0;
	virtual void ResetParticle(Particle* particle) = 0;
protected:
	float GetRandomFloat(float lowBound, float highBound)
	{
		if (lowBound >= highBound)
			return lowBound;
		float f = (rand() % 10000) * 0.0001f;

		return f * (highBound - lowBound) + lowBound;
	}

	void GetRandomVector(D3DXVECTOR3 *pOut, D3DXVECTOR3 *pMin,
		D3DXVECTOR3 *pMax)
	{
		pOut->x = GetRandomFloat(pMin->x, pMax->x);
		pOut->y = GetRandomFloat(pMin->y, pMax->y);
		pOut->z = GetRandomFloat(pMin->z, pMax->z);
	}

	DWORD FloatToDword(float val)
	{
		float *a = &val;
		DWORD *b = (DWORD*)a;
		DWORD c = *b;
		return c;
	}
};
#endif