#ifndef __PARTICLE_CONFIG_H__
#define __PARTICLE_CONFIG_H__
#include <string>
#include <vector>

class ParticleConfig
{
public:
	DWORD color;
	std::vector<std::string> textureNames;
	int lifeTime;
};
#endif