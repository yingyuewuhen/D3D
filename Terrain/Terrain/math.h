#if !defined(__MATH_H__)
#define __MATH_H__


class Math
{
public:
	static float Lerp(float a, float b, float t);
	static float SQR(float x)
	{
		return x * x;
	}
	static float MAX(float a, float b)
	{
		return a > b ? a : b;
	}
};
#endif