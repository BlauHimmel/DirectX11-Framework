#include "D3D11Math.h"

float Random(float Min, float Max)
{
	float Random = Min + (rand() / float(RAND_MAX)) * (Max - Min);
	return Random < Min ? Min : (Random > Max ? Max : Random);
}

