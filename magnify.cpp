#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "includes/FilterCommon.h"

extern "C"
{
	static unsigned char* ScaledImage = NULL;

	const int FilterID = 0x5D0F;
	const char* FilterName = "Magnify";
	const char* FilterDescription = "nX Pixel duplication";

	bool ComparisonThreshold = false;
	int FilterScaleX = 2;
	int FilterScaleY = 2;

	#include "includes/Init.h"

	void Apply(int argc, void** argv)
	{
		if (argc >= 4)
		{
			auto Input = ((unsigned char*)(argv[0]));
			auto srcx = *((int*)(argv[1]));
			auto srcy = *((int*)(argv[2]));
			auto scale = *((int*)(argv[3]));

			scale = scale < 1 ? 1 : scale;
			
			FilterScaleX = scale;
			FilterScaleY = scale;
			
			Init(srcx, srcy);

			for (auto y = 0; y < srcy; y++)
			{
				for (auto x = 0; x < srcx; x++)
				{
					WriteMagnify(Input, ScaledImage, srcx, srcy, x, y);
				}
			}
		}
	}
}


