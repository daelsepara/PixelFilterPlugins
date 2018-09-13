#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "includes/FilterCommon.h"
#include "includes/hq.h"

extern "C"
{
	static unsigned char* ScaledImage = NULL;

	const int FilterID = 0x5D1C;
	const char* FilterName = "HQ 2Xn";
	const char* FilterDescription = "Maxim Stepin's High Quality 2Xn Magnification";

	bool ComparisonThreshold = false;
	const int FilterScaleX = 2;
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
			scale = scale > 4 ? 4 : scale;
			
			FilterScaleY = scale;
			
			Init(srcx, srcy);

			int P[17];

			for (auto y = 0; y < srcy; y++)
			{
				for (auto x = 0; x < srcx; x++)
				{
					auto c0 = CLR(Input, srcx, srcy, x, y, -1, -1);
					auto c1 = CLR(Input, srcx, srcy, x, y, 0, -1);
					auto c2 = CLR(Input, srcx, srcy, x, y, 1, -1);
					auto c3 = CLR(Input, srcx, srcy, x, y, -1, 0);
					auto c4 = CLR(Input, srcx, srcy, x, y);
					auto c5 = CLR(Input, srcx, srcy, x, y, 1, 0);
					auto c6 = CLR(Input, srcx, srcy, x, y, -1, 1);
					auto c7 = CLR(Input, srcx, srcy, x, y, 0, 1);
					auto c8 = CLR(Input, srcx, srcy, x, y, 1, 1);

					unsigned char pattern = 0;
					
					if ((IsNotLike(c4, c0)))
						pattern |= 1;
					if ((IsNotLike(c4, c1)))
						pattern |= 2;
					if ((IsNotLike(c4, c2)))
						pattern |= 4;
					if ((IsNotLike(c4, c3)))
						pattern |= 8;
					if ((IsNotLike(c4, c5)))
						pattern |= 16;
					if ((IsNotLike(c4, c6)))
						pattern |= 32;
					if ((IsNotLike(c4, c7)))
						pattern |= 64;
					if ((IsNotLike(c4, c8)))
						pattern |= 128;

					switch(scale)
					{
						case 4: // 2x4
						
							Hq2x4Kernel(pattern, c0, c1, c2, c3, c4, c5, c6, c7, c8, P);

							Write16RGB(ScaledImage, srcx, srcy, x, y, 1, P[1]);
							Write16RGB(ScaledImage, srcx, srcy, x, y, 2, P[2]);
							Write16RGB(ScaledImage, srcx, srcy, x, y, 5, P[5]);
							Write16RGB(ScaledImage, srcx, srcy, x, y, 6, P[6]);
							Write16RGB(ScaledImage, srcx, srcy, x, y, 9, P[9]);
							Write16RGB(ScaledImage, srcx, srcy, x, y, 10, P[10]);
							Write16RGB(ScaledImage, srcx, srcy, x, y, 13, P[13]);
							Write16RGB(ScaledImage, srcx, srcy, x, y, 14, P[14]);

							break;
							
						case 3: // 2x3
						
							Hq2x3Kernel(pattern, c0, c1, c2, c3, c4, c5, c6, c7, c8, P);

							Write9RGB(ScaledImage, srcx, srcy, x, y, 1, P[1]);
							Write9RGB(ScaledImage, srcx, srcy, x, y, 2, P[2]);
							Write9RGB(ScaledImage, srcx, srcy, x, y, 4, P[4]);
							Write9RGB(ScaledImage, srcx, srcy, x, y, 5, P[5]);
							Write9RGB(ScaledImage, srcx, srcy, x, y, 7, P[7]);
							Write9RGB(ScaledImage, srcx, srcy, x, y, 8, P[8]);
							
							break;
							
						default: // 2x2
					
							Hq2xKernel(pattern, c0, c1, c2, c3, c4, c5, c6, c7, c8, P);

							for (auto Pixel = 1; Pixel < 5; Pixel++)
							{
								Write4RGB(ScaledImage, srcx, srcy, x, y, Pixel, P[Pixel]);
							}
							
							break;
					}
				}
			}
		}
	}
}

