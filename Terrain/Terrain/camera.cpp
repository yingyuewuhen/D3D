#include <cmath>
#include "camera.h"

const float Camera::DEFAULT_FOVX = 80.0f;
const float Camera::DEFAULT_ROTATION_SPEED = 0.15f;
const float Camera::DEFAULT_ZFAR = 1000.0f;
const float Camera::DEFAULT_ZNEAR = 1.0f;

const D3DXVECTOR3 Camera::WORLD_XAXIS(1.0f, 0.0f, 0.0f);
const D3DXVECTOR3 Camera::WORLD_YAXIS(0.0f, 1.0f, 0.0f);
const D3DXVECTOR3 Camera::WORLD_ZAXIS(0.0f, 0.0f, 1.0f);

Camera::Camera()
{
    mode = CAMERA_MODE_FIRST_PERSON;

	m_frustrum.resize(6);

    fovx = DEFAULT_FOVX;
    znear = DEFAULT_ZNEAR;
    zfar = DEFAULT_ZFAR;
    rotationSpeed = DEFAULT_ROTATION_SPEED;

    m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
    m_up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    m_look = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

    D3DXMatrixIdentity(&viewMatrix);
    D3DXMatrixIdentity(&projMatrix);

    m_accumPitchDegrees = 0.0f;
}

Camera::~Camera()
{
}

void Camera::lookAt(const D3DXVECTOR3 &eye, const D3DXVECTOR3 &at, const D3DXVECTOR3 &up)
{
    this->m_pos = eye;

    m_look = at - eye;
    D3DXVec3Normalize(&m_look, &m_look);

    D3DXVec3Cross(&m_right, &up, &m_look);
    D3DXVec3Normalize(&m_right, &m_right);

    D3DXVec3Cross(&m_up, &m_look, &m_right);
    D3DXVec3Normalize(&m_up, &m_up);
    D3DXVec3Normalize(&m_right, &m_right);

    updateViewMatrix(false);

    m_accumPitchDegrees = D3DXToDegree(-asinf(viewMatrix(1,2)));
}

void Camera::move(float dx, float dy, float dz)
{
    D3DXVECTOR3 forwards;

    switch (mode)
    {
    case CAMERA_MODE_FIRST_PERSON:
        D3DXVec3Cross(&forwards, &m_right, &WORLD_YAXIS);
        D3DXVec3Normalize(&forwards, &forwards);
        break;

    case CAMERA_MODE_FLIGHT_SIM:
        forwards = m_look;
         break;
     }

    m_pos += m_right * dx;
    m_pos += m_up * dy;
    m_pos += forwards * dz;
    
    updateViewMatrix(false);
}

void Camera::move(const D3DXVECTOR3 &direction, float velocity)
{
    m_pos += direction * velocity;
    updateViewMatrix(false);
}

void Camera::mouseRotate(float dx, float dy, float dz)
{
    rotate(dx * rotationSpeed, dy * rotationSpeed, dz * rotationSpeed);
}

void Camera::perspective(float fovx, float aspect, float znear, float zfar)
{
    // We construct a projection matrix based on the horizontal field of view
    // 'fovx' rather than the more traditional vertical field of view 'fovy'.

    float e = 1.0f / tanf(D3DXToRadian(fovx) / 2.0f);
    float aspectInv = 1.0f / aspect;
    float fovy = 2.0f * atanf(aspectInv / e);
    float xScale = 1.0f / tanf(0.5f * fovy);
    float yScale = xScale / aspectInv;

    projMatrix(0,0) = xScale;
    projMatrix(1,0) = 0.0f;
    projMatrix(2,0) = 0.0f;
    projMatrix(3,0) = 0.0f;

    projMatrix(0,1) = 0.0f;
    projMatrix(1,1) = yScale;
    projMatrix(2,1) = 0.0f;
    projMatrix(3,1) = 0.0f;

    projMatrix(0,2) = 0.0f;
    projMatrix(1,2) = 0.0f;
    projMatrix(2,2) = zfar / (zfar - znear);
    projMatrix(3,2) = -znear * zfar / (zfar - znear);

    projMatrix(0,3) = 0.0f;
    projMatrix(1,3) = 0.0f;
    projMatrix(2,3) = 1.0f;
    projMatrix(3,3) = 0.0f;

    this->fovx = fovx;
    this->znear = znear;
    this->zfar = zfar;
}

void Camera::reset()
{
    m_right = WORLD_XAXIS;
    m_up = WORLD_YAXIS;
    m_look = WORLD_ZAXIS;
    m_accumPitchDegrees = 0.0f;
    updateViewMatrix(false);
}

void Camera::rotate(float head, float pitch, float roll)
{
    roll = -roll;

    switch (mode)
    {
    case CAMERA_MODE_FIRST_PERSON:
        rotateFirstPerson(head, pitch);
        break;

    case CAMERA_MODE_FLIGHT_SIM:
        rotateFlightSim(head, pitch, roll);
        break;
    }

    updateViewMatrix(true);
}

void Camera::rotateFlightSim(float head, float pitch, float roll)
{
    head = D3DXToRadian(head);
    pitch = D3DXToRadian(pitch);
    roll = D3DXToRadian(roll);

    D3DXMATRIX rotMtx;
    D3DXVECTOR4 result;

    if (head != 0.0f)
    {
        D3DXMatrixRotationAxis(&rotMtx, &m_up, head);
        D3DXVec3Transform(&result, &m_right, &rotMtx);
        m_right = D3DXVECTOR3(result.x, result.y, result.z);
        D3DXVec3Transform(&result, &m_look, &rotMtx);
        m_look = D3DXVECTOR3(result.x, result.y, result.z);
    }

    if (pitch != 0.0f)
    {
        D3DXMatrixRotationAxis(&rotMtx, &m_right, pitch);
        D3DXVec3Transform(&result, &m_up, &rotMtx);
        m_up = D3DXVECTOR3(result.x, result.y, result.z);
        D3DXVec3Transform(&result, &m_look, &rotMtx);
        m_look = D3DXVECTOR3(result.x, result.y, result.z);
    }

    if (roll != 0.0f)
    {
        D3DXMatrixRotationAxis(&rotMtx, &m_look, roll);
        D3DXVec3Transform(&result, &m_right, &rotMtx);
        m_right = D3DXVECTOR3(result.x, result.y, result.z);
        D3DXVec3Transform(&result, &m_up, &rotMtx);
        m_up = D3DXVECTOR3(result.x, result.y, result.z);
    }
}

void Camera::rotateFirstPerson(float head, float pitch)
{
    m_accumPitchDegrees += pitch;

    if (m_accumPitchDegrees > 90.0f)
    {
        pitch = 90.0f - (m_accumPitchDegrees - pitch);
        m_accumPitchDegrees = 90.0f;
    }

    if (m_accumPitchDegrees < -90.0f)
    {
        pitch = -90.0f - (m_accumPitchDegrees - pitch);
        m_accumPitchDegrees = -90.0f;
    }

    head = D3DXToRadian(head);
    pitch = D3DXToRadian(pitch);

    D3DXMATRIX rotMtx;
    D3DXVECTOR4 result;

    // Rotate camera's existing x and z axes about the world y axis.
    if (head != 0.0f)
    {
        D3DXMatrixRotationY(&rotMtx, head);
        D3DXVec3Transform(&result, &m_right, &rotMtx);
        m_right = D3DXVECTOR3(result.x, result.y, result.z);
        D3DXVec3Transform(&result, &m_look, &rotMtx);
        m_look = D3DXVECTOR3(result.x, result.y, result.z);
    }

    // Rotate camera's existing y and z axes about its existing x axis.
    if (pitch != 0.0f)
    {
        D3DXMatrixRotationAxis(&rotMtx, &m_right, pitch);
        D3DXVec3Transform(&result, &m_up, &rotMtx);
        m_up = D3DXVECTOR3(result.x, result.y, result.z);
        D3DXVec3Transform(&result, &m_look, &rotMtx);
        m_look = D3DXVECTOR3(result.x, result.y, result.z);
    }
}

void Camera::setMode(CameraMode newMode)
{
    if (mode == CAMERA_MODE_FLIGHT_SIM && newMode == CAMERA_MODE_FIRST_PERSON)
    {
        lookAt(m_pos, m_pos + m_look, WORLD_YAXIS);
    }

    mode = newMode;
}

void Camera::setPosition(float x, float y, float z)
{
    m_pos.x = x;
    m_pos.y = y;
    m_pos.z = z;

    updateViewMatrix(false);
}

void Camera::updateViewMatrix(bool orthogonalizeAxes)
{
    if (orthogonalizeAxes)
    {
        // Regenerate the camera's local axes to orthogonalize them.
        D3DXVec3Normalize(&m_look, &m_look);
        D3DXVec3Cross(&m_up, &m_look, &m_right);
        D3DXVec3Normalize(&m_up, &m_up);
        D3DXVec3Cross(&m_right, &m_up, &m_look);
        D3DXVec3Normalize(&m_right, &m_right);
    }

    // Reconstruct the view matrix.

    viewMatrix(0,0) = m_right.x;
    viewMatrix(1,0) = m_right.y;
    viewMatrix(2,0) = m_right.z;
    viewMatrix(3,0) = -D3DXVec3Dot(&m_right, &m_pos);

    viewMatrix(0,1) = m_up.x;
    viewMatrix(1,1) = m_up.y;
    viewMatrix(2,1) = m_up.z;
    viewMatrix(3,1) = -D3DXVec3Dot(&m_up, &m_pos);

    viewMatrix(0,2) = m_look.x;
    viewMatrix(1,2) = m_look.y;
    viewMatrix(2,2) = m_look.z;
    viewMatrix(3,2) = -D3DXVec3Dot(&m_look, &m_pos);

    viewMatrix(0,3) = 0.0f;
    viewMatrix(1,3) = 0.0f;
    viewMatrix(2,3) = 0.0f;
    viewMatrix(3,3) = 1.0f;



	//º∆À„æµœÒæÿ’Û
// 	D3DXVECTOR3 m_reflectPosition = m_pos;
// 	m_reflectPosition.y = 2 * m_refPlaneHeight - m_reflectPosition.y;
// 
// 	D3DXVECTOR3 refTarget = m_look;
// 	refTarget.y = 2 * m_refPlaneHeight - refTarget.y;
// 
// 	D3DXVECTOR3 refUp = m_up;
// 
// 	D3DXMATRIX diff;
// 	D3DXVec3TransformCoord(&refUp, &D3DXVECTOR3(0, 1, 0), &diff);

	D3DXMatrixReflect(&m_reflectMatrix, &m_refPlane);
}

const D3DXVECTOR3 & Camera::getPosition() const
{
	return m_pos;
}

const std::vector<Plane> & Camera::getFrustrum() const
{
	return m_frustrum;
}

void Camera::recalculateFrustrum()
{
	D3DXMATRIX viewProj = viewMatrix * projMatrix;

	//◊Û≤√ºı√Ê
	m_frustrum[VF_LEFT_PLANE].normal.x = viewProj._11 + viewProj._41;
	m_frustrum[VF_LEFT_PLANE].normal.y = viewProj._12 + viewProj._42;
	m_frustrum[VF_LEFT_PLANE].normal.z = viewProj._13 + viewProj._43;
	m_frustrum[VF_LEFT_PLANE].d = viewProj._14 + viewProj._44;

	//”“≤√ºı√Ê
	m_frustrum[VF_RIGHT_PLANE].normal.x = viewProj._11 - viewProj._41;
	m_frustrum[VF_RIGHT_PLANE].normal.y = viewProj._12 - viewProj._42;
	m_frustrum[VF_RIGHT_PLANE].normal.z = viewProj._13 - viewProj._43;
	m_frustrum[VF_RIGHT_PLANE].d = viewProj._14 - viewProj._44;

	//…œ≤√ºı√Ê

}
