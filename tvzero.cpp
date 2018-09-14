#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "includes/FilterCommon.h"

extern "C"
{
	static unsigned char* ScaledImage = NULL;

	const int FilterID = 0x5D24;
	const char* FilterName = "TV Zero";
	const char* FilterDescription = "No-scaling TV-like effect using interlacing and gamma reduction";

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
			auto scale = *((int*)(argv[3]));

			scale = scale < 1 ? 1 : scale;
			scale = scale > 4 ? 4 : scale;

			auto Channels = 3;

			Init(srcx, srcy);

			for (auto y = 0; y < srcy; y++)
			{
				for (auto x = 0; x < srcx; x++)
				{
					auto pixel = CLR(Input, srcx, srcy, x, y);
					
					auto R = Red(pixel);
					auto G = Green(pixel);
					auto B = Blue(pixel);

					auto subPixel = RGBINT((R * 5) >> 3, (G * 5) >> 3, (B * 5) >> 3);
					auto subPixel2 = RGBINT((R * 5) >> 4, (G * 5) >> 4, (B * 5) >> 4);
					auto subPixel3 = RGBINT((R * 5) >> 5, (G * 5) >> 5, (B * 5) >> 5);

					auto dst = (y * srcx + x) * Channels;
					
					auto row = y % scale;
					
					switch(row)
					{
						case 1:
							
							ScaledImage[dst] = Red(subPixel);
							ScaledImage[dst + 1] = Green(subPixel);
							ScaledImage[dst + 2] = Blue(subPixel);
							
							break;

						case 2:
							
							ScaledImage[dst] = Red(subPixel2);
							ScaledImage[dst + 1] = Green(subPixel2);
							ScaledImage[dst + 2] = Blue(subPixel2);

							break;
						
						case 3:
						
							ScaledImage[dst] = Red(subPixel3);
							ScaledImage[dst + 1] = Green(subPixel3);
							ScaledImage[dst + 2] = Blue(subPixel3);
													
							break;
						
						default:
							
							ScaledImage[dst] = Red(pixel);
							ScaledImage[dst + 1] = Green(pixel);
							ScaledImage[dst + 2] = Blue(pixel);
							
							break;
					}
				}
			}
		}
	}
}


