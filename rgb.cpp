#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "includes/FilterCommon.h"

extern "C"
{
	static unsigned char* ScaledImage = NULL;

	const int FilterID = 0x5D0B;
	const char* FilterName = "RGB nX";
	const char* FilterDescription = "Dot-Matrix Printer Effect";

	bool ComparisonThreshold = false;
	int FilterScaleX = 1;
	int FilterScaleY = 1;

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
			
			FilterScaleX = scale;
			FilterScaleY = scale;
			
			auto Channels = 3;
			
			Init(srcx, srcy);
			
			int P[17];

			for (auto y = 0; y < srcy; y++)
			{
				for (auto x = 0; x < srcx; x++)
				{
					auto pixel = CLR(Input, srcx, srcy, x, y);
					
					auto oddrow = (y % 2) > 0;
					auto oddcol = (x % 2) > 0;
					
					auto dst = (y * srcx + x) * Channels;
					
					switch(scale)
					{
						case 2: // x2
							
							P[1] = RGBINT(Red(pixel), 0, 0);
							P[2] = RGBINT(0, Green(pixel), 0);
							P[3] = RGBINT(0, 0, Blue(pixel));
							P[4] = pixel;

							for (auto Pixel = 1; Pixel < 5; Pixel++)
							{
								Write4RGB(ScaledImage, srcx, srcy, x, y, Pixel, P[Pixel]);
							}
							
							break;

						case 3: // x3
						
							P[1] = pixel;
							P[2] = RGBINT(0, Green(pixel), 0);
							P[3] = RGBINT(0, 0, Blue(pixel));
							P[4] = RGBINT(0, 0, Blue(pixel));
							P[5] = pixel;
							P[6] = RGBINT(Red(pixel), 0, 0);
							P[7] = RGBINT(Red(pixel), 0, 0);
							P[8] = RGBINT(0, Green(pixel), 0);
							P[9] = pixel;

							for (auto Pixel = 1; Pixel < 10; Pixel++)
							{
								Write9RGB(ScaledImage, srcx, srcy, x, y, Pixel, P[Pixel]);
							}
							
							break;

						case 4: // x4
						
							P[1] = P[2] = P[5] = P[6] = pixel; 
							P[3] = P[4] = P[7] = P[8] = RGBINT(0, Green(pixel), 0);
							P[9] = P[10] = P[13] = P[14] = RGBINT(0, 0, Blue(pixel));
							P[11] = P[12] = P[15] = P[16] = RGBINT(Red(pixel), 0, 0);
							
							for (auto Pixel = 1; Pixel < 17; Pixel++)
							{
								Write16RGB(ScaledImage, srcx, srcy, x, y, Pixel, P[Pixel]);
							}
							
							break;
							

						default: // x1
							
							if (oddrow && oddcol)
							{
								ScaledImage[dst] = 0x00;
								ScaledImage[dst + 1] = 0x00;
								ScaledImage[dst + 2] = Blue(pixel);
							}
							else if (!oddrow && !oddcol)
							{
								ScaledImage[dst] = Red(pixel);
								ScaledImage[dst + 1] = Green(pixel);
								ScaledImage[dst + 2] = Blue(pixel);
							}
							else if (oddrow)
							{
								ScaledImage[dst] = 0x00;
								ScaledImage[dst + 1] = Green(pixel);
								ScaledImage[dst + 2] = 0x00;
							}
							else
							{
								ScaledImage[dst] = Red(pixel);
								ScaledImage[dst + 1] = 0x00;
								ScaledImage[dst + 2] = 0x00;
							}
							
							break;							
					}
				}
			}
		}
	}
}


