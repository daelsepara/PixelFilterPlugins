#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "includes/FilterCommon.h"

extern "C"
{
	static unsigned char* ScaledImage = NULL;

	const int FilterID = 0x5D12;
	const char* FilterName = "Vertical Scanlines nX";
	const char* FilterDescription = "Vertical Scanlines nX";

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

					auto R = Red(pixel) >> 1;
					auto G = Green(pixel) >> 1;
					auto B = Blue(pixel) >> 1;

					auto subPixel = RGBINT(R, G, B);
					auto subPixel2 = RGBINT(R >> 1, G >> 1, B >> 1);

					auto odd = (x % 2) > 0;
					
					auto dst = (y * srcx + x) * Channels;
					
					switch(scale)
					{
						case 2: // x2
							
							P[1] = P[3] = pixel;
							P[2] = P[4] = subPixel;

							for (auto Pixel = 1; Pixel < 5; Pixel++)
							{
								Write4RGB(ScaledImage, srcx, srcy, x, y, Pixel, P[Pixel]);
							}
							
							break;

						case 3: // x3
						
							P[1] = P[4] = P[7] = pixel;
							P[2] = P[5] = P[8] = subPixel;
							P[3] = P[6] = P[9] = subPixel2;

							for (auto Pixel = 1; Pixel < 10; Pixel++)
							{
								Write9RGB(ScaledImage, srcx, srcy, x, y, Pixel, P[Pixel]);
							}
							
							break;
							
						case 4: // x4
						
							P[1] = P[5] = P[9] = P[13] = pixel;
							P[2] = P[6] = P[10] = P[14] = pixel;
							P[3] = P[7] = P[11] = P[15] = subPixel;
							P[4] = P[8] = P[12] = P[16] = subPixel2;

							for (auto Pixel = 1; Pixel < 17; Pixel++)
							{
								Write16RGB(ScaledImage, srcx, srcy, x, y, Pixel, P[Pixel]);
							}
							
							break;
							
						default: // x1
							
							ScaledImage[dst] = odd ? Red(subPixel) : Red(pixel);
							ScaledImage[dst + 1] = odd ? Green(subPixel) : Green(pixel);
							ScaledImage[dst + 2] = odd ? Blue(subPixel) : Blue(pixel);
							
							break;
					}
				}
			}
		}
	}
}
