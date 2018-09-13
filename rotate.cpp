#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "includes/FilterCommon.h"
#include "includes/Rotate.h"

extern "C"
{
	static unsigned char* ScaledImage = NULL;

	const int FilterID = 0x5D22;
	const char* FilterName = "Rotate (90, 180, 270)";
	const char* FilterDescription = "Rotation without scaling (90, 180, 270 degrees)";

	bool ComparisonThreshold = false;
	const int FilterScaleX = 1;
	const int FilterScaleY = 1;

	#include "includes/Init.h"

	void Apply(int argc, void** argv)
	{
		if (argc >= 4)
		{
			auto Input = ((unsigned char*)(argv[0]));
			auto srcx = *((int*)(argv[1]));
			auto srcy = *((int*)(argv[2]));
			auto rotate = *((int*)(argv[3]));
			
			rotate = rotate < 0 ? 0 : rotate;
			rotate = rotate > 2 ? 2 : rotate;
			
			Init(srcx, srcy);

			switch(rotate)
			{
				case 1:
					
					Rotate180(ScaledImage, Input, srcx, srcy);
					
					break;
				
				case 2:
				
					Rotate270(ScaledImage, Input, srcx, srcy);

					std::swap(_SizeX, _SizeY);
			
					break;
					
				default:
				
					Rotate90(ScaledImage, Input, srcx, srcy);

					std::swap(_SizeX, _SizeY);
					
					break;
			}
		}
	}
}
