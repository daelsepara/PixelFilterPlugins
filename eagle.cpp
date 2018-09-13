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

	const int FilterID = 0x5D08;
	const char* FilterName = "Eagle 2X";
	const char* FilterDescription = "Eagle nX Family of Filters";

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
			
			int C[9];
			int P[10];
			
			for (auto y = 0; y < srcy; y++)
			{
				for (auto x = 0; x < srcx; x++)
				{
					//   C         P
					//
					//          +-----+
					// 0 1 2    |1|2|3|
					//  \|/     |-+-+-|
					// 3-4-5 => |4|5|6|
					//  /|\     |-+-+-|
					// 6 7 8    |7|8|9|
					//          +-----+
					
					C[0] = CLR(Input, srcx, srcy, x, y, -1, -1);
					C[1] = CLR(Input, srcx, srcy, x, y, 0, -1);
					C[2] = CLR(Input, srcx, srcy, x, y, 1, -1);
					C[3] = CLR(Input, srcx, srcy, x, y, -1, 0);
					C[4] = CLR(Input, srcx, srcy, x, y, 0, 0);
					C[5] = CLR(Input, srcx, srcy, x, y, 1, 0);
					C[6] = CLR(Input, srcx, srcy, x, y, -1, 1);
					C[7] = CLR(Input, srcx, srcy, x, y, 0, 1);
					C[8] = CLR(Input, srcx, srcy, x, y, 1, 1);

					switch(scale)
					{
						case 3:
							
							P[1] = (IsLike(C[0], C[1]) && IsLike(C[0], C[3])) ? Interpolate(C[0], C[1], C[3]) : C[4];
							P[2] = (IsLike(C[0], C[1]) && IsLike(C[0], C[3]) && IsLike(C[2], C[1]) && IsLike(C[2], C[5])) ? Interpolate(Interpolate(C[0], C[1], C[3]), Interpolate(C[2], C[1], C[5])) : C[4];
							P[3] = (IsLike(C[2], C[1]) && IsLike(C[2], C[5])) ? Interpolate(C[2], C[1], C[5]) : C[4];
							P[4] = (IsLike(C[0], C[1]) && IsLike(C[0], C[3]) && IsLike(C[6], C[7]) && IsLike(C[6], C[3])) ? Interpolate(Interpolate(C[0], C[1], C[3]), Interpolate(C[6], C[3], C[7])) : C[4];
							P[5] = C[4];
							P[6] = (IsLike(C[2], C[1]) && IsLike(C[2], C[5]) && IsLike(C[8], C[5]) && IsLike(C[8], C[7])) ? Interpolate(Interpolate(C[2], C[1], C[5]), Interpolate(C[8], C[5], C[7])) : C[4];
							P[7] = (IsLike(C[6], C[3]) && IsLike(C[6], C[7])) ? Interpolate(C[6], C[3], C[7]) : C[4];
							P[8] = (IsLike(C[6], C[7]) && IsLike(C[6], C[3]) && IsLike(C[8], C[5]) && IsLike(C[8], C[7])) ? Interpolate(Interpolate(C[6], C[7], C[3]), Interpolate(C[8], C[5], C[7])) : C[4];
							P[9] = (IsLike(C[8], C[5]) && IsLike(C[8], C[7])) ? Interpolate(C[8], C[5], C[7]) : C[4];

							for (auto Pixel = 1; Pixel < 10; Pixel++)
							{
								Write9RGB(ScaledImage, srcx, srcy, x, y, Pixel, P[Pixel]);
							}
						
							break;
							
						default:
						
							P[1] = (IsLike(C[1], C[0]) && IsLike(C[1], C[3])) ? Interpolate(C[1], C[0], C[3]) : C[4];
							P[2] = (IsLike(C[2], C[1]) && IsLike(C[2], C[5])) ? Interpolate(C[2], C[1], C[5]) : C[4];
							P[3] = (IsLike(C[6], C[3]) && IsLike(C[6], C[7])) ? Interpolate(C[6], C[3], C[7]) : C[4];
							P[4] = (IsLike(C[7], C[5]) && IsLike(C[7], C[8])) ? Interpolate(C[7], C[5], C[8]) : C[4];

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
