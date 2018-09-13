#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "includes/FilterCommon.h"
#include "includes/Flip.h"

extern "C"
{
	static unsigned char* ScaledImage = NULL;

	const int FilterID = 0x5D21;
	const char* FilterName = "Flip (Horizontal, Vertical)";
	const char* FilterDescription = "Flip image horizontally or vertically without scaling";

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
			auto flip = *((int*)(argv[3]));
			
			flip = flip < 0 ? 0 : flip;
			flip = flip > 1 ? 1 : flip;
			
			Init(srcx, srcy);

			auto Channels = 3;

			Copy(ScaledImage, Input, srcx * srcy * Channels);

			switch(flip)
			{
				case 1:
				
					FlipUD(ScaledImage, srcx, srcy);
					
					break;
					
				default:
				
					FlipLR(ScaledImage, srcx, srcy);
					
					break;
			}
		}
	}
}



