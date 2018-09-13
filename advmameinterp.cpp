#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "includes/FilterCommon.h"
#include "includes/Interpolate.h"

extern "C"
{
	static unsigned char* ScaledImage = NULL;

	const int FilterID = 0x5D0D;
	const char* FilterName = "AdvanceMame";
	const char* FilterDescription = "AdvanceMame scaling using interpolation";

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
			scale = scale > 3 ? 3 : scale;
			
			FilterScaleX = scale;
			FilterScaleY = scale;

			Init(srcx, srcy);
			
			int P[10];
			
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

					P[1] = P[2] = P[3] = P[4] = P[5] = P[6] = P[7] = P[8] = P[9] = c4;
					
					switch(scale)
					{
						case 3: // x3
							
							if (IsNotLike(c1, c7) && IsNotLike(c3, c5)) {
								if (IsLike(c3, c1)) {
									P[1] = Interpolate(Interpolate(c3, c1), c4, 5, 3);
								}
								if (IsLike(c1, c5)) {
									P[3] = Interpolate(Interpolate(c5, c1), c4, 5, 3);
								}
								if (IsLike(c3, c7)) {
									P[7] = Interpolate(Interpolate(c3, c7), c4, 5, 3);
								}
								if (IsLike(c7, c5)) {
									P[9] = Interpolate(Interpolate(c7, c5), c4, 5, 3);
								}

								if (
									(IsLike(c3, c1) && IsNotLike(c4, c2)) &&
									(IsLike(c5, c1) && IsNotLike(c4, c0))
									)
									P[2] = Interpolate(c1, c3, c5);
								else if (IsLike(c3, c1) && IsNotLike(c4, c2))
									P[2] = Interpolate(c3, c1);
								else if (IsLike(c5, c1) && IsNotLike(c4, c0))
									P[2] = Interpolate(c5, c1);

								if (
									(IsLike(c3, c1) && IsNotLike(c4, c6)) &&
									(IsLike(c3, c7) && IsNotLike(c4, c0))
									)
									P[4] = Interpolate(c3, c1, c7);
								else if (IsLike(c3, c1) && IsNotLike(c4, c6))
									P[4] = Interpolate(c3, c1);
								else if (IsLike(c3, c7) && IsNotLike(c4, c0))
									P[4] = Interpolate(c3, c7);

								if (
									(IsLike(c5, c1) && IsNotLike(c4, c8)) &&
									(IsLike(c5, c7) && IsNotLike(c4, c2))
									)
									P[6] = Interpolate(c5, c1, c7);
								else if (IsLike(c5, c1) && IsNotLike(c4, c8))
									P[6] = Interpolate(c5, c1);
								else if (IsLike(c5, c7) && IsNotLike(c4, c2))
									P[6] = Interpolate(c5, c7);

								if (
									(IsLike(c3, c7) && IsNotLike(c4, c8)) &&
									(IsLike(c5, c7) && IsNotLike(c4, c6))
									)
									P[8] = Interpolate(c7, c3, c5);
								else if (IsLike(c3, c7) && IsNotLike(c4, c8))
									P[8] = Interpolate(c3, c7);
								else if (IsLike(c5, c7) && IsNotLike(c4, c6))
									P[8] = Interpolate(c5, c7);
							}

							for (auto Pixel = 1; Pixel < 10; Pixel++)
							{
								Write9RGB(ScaledImage, srcx, srcy, x, y, Pixel, P[Pixel]);
							}
						
							break;
						
						default: // x2
							
							if (IsNotLike(c1, c7) && IsNotLike(c3, c5))
							{
								P[1] = (IsLike(c3, c1)) ? Interpolate(Interpolate(c1, c3), c4, 5, 3) : c4;
								P[2] = (IsLike(c5, c1)) ? Interpolate(Interpolate(c1, c5), c4, 5, 3) : c4;
								P[3] = (IsLike(c3, c7)) ? Interpolate(Interpolate(c7, c3), c4, 5, 3) : c4;
								P[3] = (IsLike(c5, c7)) ? Interpolate(Interpolate(c7, c5), c4, 5, 3) : c4;
							}

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

