#pragma once
#include <d3dx9.h>
struct VertexPositionTex
{
	VertexPositionTex(D3DXVECTOR3 _pos, D3DXVECTOR2 _uv)
		:pos(_pos), uv(_uv)
	{}

	VertexPositionTex(float _x, float _y, float _z,
		float _u, float _v)
		:pos(_x, _y, _z), uv(_u, _v)
	{}

	D3DXVECTOR3 pos;
	D3DXVECTOR2 uv;


	const static DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;
};