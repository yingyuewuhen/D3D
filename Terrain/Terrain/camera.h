
#if !defined(CAMERA_H)
#define CAMERA_H

#include <vector>

#include <d3dx9.h>
#include "plane.h"


enum VFPLANES
{
	//垓加受中
	VF_FAR_PLANE = 0,
	//除加受中
	VF_NEAR_PLANE,
	//恣加受中
	VF_LEFT_PLANE,
	//嘔加受中
	VF_RIGHT_PLANE,
	//和加受中
	VF_BOTTOM_PLANE,
	//貧加受中
	VF_TOP_PLANE,

	//
	VF_PLANE_COUNT
};

class Camera
{
public:
    static const float DEFAULT_FOVX;
    static const float DEFAULT_ROTATION_SPEED;
    static const float DEFAULT_ZFAR;
    static const float DEFAULT_ZNEAR;

    static const D3DXVECTOR3 WORLD_XAXIS;
    static const D3DXVECTOR3 WORLD_YAXIS;
    static const D3DXVECTOR3 WORLD_ZAXIS;

    enum CameraMode
    {
        CAMERA_MODE_FIRST_PERSON,
        CAMERA_MODE_FLIGHT_SIM,
    };

    CameraMode mode;
    float fovx, znear, zfar, rotationSpeed;
    D3DXVECTOR3 m_pos, m_right, m_up, m_look;
  

    Camera();
    ~Camera();

    void lookAt(const D3DXVECTOR3 &eye, const D3DXVECTOR3 &at, const D3DXVECTOR3 &up);
    void mouseRotate(float head, float pitch, float roll);
    void move(float dx, float dy, float dz);
    void move(const D3DXVECTOR3 &direction, float velocity);
    void perspective(float fovx, float aspect, float znear, float zfar);
    void reset();
    void rotate(float head, float pitch, float roll);
    void setMode(CameraMode newMode);
    void setPosition(float x, float y, float z);
    
	const D3DXMATRIX &getViewMatrix() const
	{
		return viewMatrix;
	}

	const D3DXMATRIX &getProjMatrix() const
	{
		return projMatrix;
	}
	const D3DXMATRIX &getReflectMatrix() const
	{
		return m_reflectMatrix;
	}

	const D3DXVECTOR3 &getPosition() const;
	const D3DXVECTOR3 &getLook() const
	{
		return m_look;
	}

	
	const std::vector<Plane> &getFrustrum() const;

	void SetReflectPlane(D3DXPLANE reflectPlane){ m_refPlane = reflectPlane; }

	D3DXVECTOR3 GetReflectPos(){ return m_reflectPosition; }

private:
	std::vector<Plane> m_frustrum;

	D3DXMATRIX viewMatrix, projMatrix;
	D3DXMATRIX m_reflectMatrix;

	D3DXVECTOR3 m_reflectPosition;

	D3DXPLANE m_refPlane;
private:
    void rotateFlightSim(float head, float pitch, float roll);
    void rotateFirstPerson(float head, float pitch);
    void updateViewMatrix(bool orthogonalizeAxes);
    
	void recalculateFrustrum();

    float m_accumPitchDegrees;
};

#endif