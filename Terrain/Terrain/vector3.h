#ifndef __VECTOR3_H__
#define __VECTOR3_H__

template<class T>
class Vector3
{
public:
	Vector3()
		:x(0), y(0), z(0)
	{}
	Vector3(T _x, T _y, T _z)
		:x(_x), y(_y), z(_z)
	{}
	Vector3(const Vector3<T> &other)
		:x(other.x), y(other.y), z(other.z)
	{
	}
	T x;
	T y;
	T z;
};

typedef Vector3<float> vector3df;
typedef Vector3<int> vector3di;


#endif