#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "includes/FilterCommon.h"
#include "includes/xbrz.h"

extern "C"
{
	static unsigned char* ScaledImage = NULL;

	const int FilterID = 0x5D03;
	const char* FilterName = "XBRZ";
	const char* FilterDescription = "XBRZnX family of filters";

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

			scale = scale < 2 ? 2 : scale;
			scale = scale > 6 ? 6 : scale;
			
			FilterScaleX = scale;
			FilterScaleY = scale;
			
			Init(srcx, srcy);

			auto src = (uint32_t*)malloc(srcx * srcy * sizeof(uint32_t));
			auto dst = (uint32_t*)malloc(SizeX() * SizeY() * sizeof(uint32_t));

			xbrz::Copy(src, Input, srcx, srcy);
			xbrz::scale(scale, src, dst, srcx, srcy, xbrz::ColorFormat::ARGB, xbrz::ScalerCfg(), 0,  srcy);
			xbrz::Copy(ScaledImage, dst, SizeX(), SizeY());

			free(dst);
			free(src);
		}
	}
}
