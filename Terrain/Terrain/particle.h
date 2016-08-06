#ifndef __PARTICLE_H__
#define __PARTICLE_H__


#include <d3dx9.h>

class Particle
{
public:
	Particle(){}
	virtual ~Particle(){}

public:
	bool isLive;  //是否存活
	float lifeTime;  //生命周期
	float age;
	
	D3DXCOLOR color;
	D3DXCOLOR colorFade;

	D3DXVECTOR3 position;
	D3DXVECTOR3 velocity;
	D3DXVECTOR3 initVelocity;

};

#endif