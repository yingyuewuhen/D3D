#include "math.h"


float Math::Lerp(float a, float b, float t)
{
	return (a + (b - a) * t);
}
