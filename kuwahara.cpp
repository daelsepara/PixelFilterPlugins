#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "includes/FilterCommon.h"

#undef max

extern "C"
{
	static unsigned char* ScaledImage = NULL;

	const int FilterID = 0x5D02;
	const char* FilterName = "Kuwahara";
	const char* FilterDescription = "Apply Kuwahara Filter (nxn window)";

	bool ComparisonThreshold = false;
	int FilterScaleX = 1;
	int FilterScaleY = 1;

	#include "includes/Init.h"

	unsigned char Clamp(double c)
	{
		return (unsigned char)(c < 0 ? 0 : (c > 255 ? 255 : c));
	}

	void varmin(double var, int srcx, int srcy, double& min, int& dstx, int& dsty)
	{
		if (var < min)
		{
			min = var;
			dstx = srcx;
			dsty = srcy;
		}
	}

	void Kuwahara(unsigned char* Input, int srcx, int srcy, int win)
	{
		int Channels = 3;
		
		int pad = (win + 1) / 2;
		int ofs = (win - 1) / 2;
		int fx = srcx + ofs;
		int fy = srcy + ofs;
		
		auto fxy = fx * fy;
		double* mean = new double[fxy];
		double* variance = new double[fxy];
		
		double sum, var;
		int n;

		for (int ys = -ofs; ys < srcy; ys++)
		{
			for (int xs = -ofs; xs < srcx; xs++)
			{
				sum = 0.0;
				var = 0.0;
				n = 0;

				for (int xf = xs; xf < xs + pad; xf++)
				{
					for (int yf = ys; yf < ys + pad; yf++)
					{
						auto val = (double)Luminance(CLR(Input, srcx, srcy, xf, yf));

						sum += val;
						var += val * val;

						n++;
					}
				}

				auto index = (ys + ofs) * fx + xs + ofs;

				mean[index] = sum / n;
				variance[index] = var - sum * mean[index];
			}
		}

		int xc = 0, yc = 0;

		double min;

		for (int y = 0; y < srcy; y++)
		{
			auto yy = y * srcx;

			for (int x = 0; x < srcx; x++)
			{
				min = std::numeric_limits<double>::max();

				auto yo = y + ofs;
				auto xo = x + ofs;
				auto yx1 = y * fx + x;
				auto yx2 = yo * fx + x;
				
				varmin(variance[yx1], x, y, min, xc, yc);
				varmin(variance[yx2], x, yo, min, xc, yc);
				varmin(variance[yx1 + ofs], xo, y, min, xc, yc);
				varmin(variance[yx2 + ofs], xo, yo, min, xc, yc);
				
				auto dst = (yy + x) * Channels;

				// YUV to RGB (ITU-R) see https://en.wikipedia.org/wiki/YUV
				auto pixel = CLR(Input, srcx, srcy, x, y);

				auto luminance = mean[yc * fx + xc] + 0.5;
				auto cr = (double)ChromaU(pixel);
				auto cb = (double)ChromaV(pixel);
				auto crr = (cr - 127.5);
				auto cbb = (cb - 127.5);

				ScaledImage[dst] = Clamp(luminance + 1.042 * crr);
				ScaledImage[dst + 1] = Clamp(luminance - 0.344 * cbb - 0.714 * crr);
				ScaledImage[dst + 2] = Clamp(luminance + 1.772 * cbb);
			}
		}

		delete[] mean;
		delete[] variance;
	}

	void Apply(int argc, void** argv)
	{
		if (argc >= 4)
		{
			auto Input = ((unsigned char*)(argv[0]));
			auto srcx = *((int*)(argv[1]));
			auto srcy = *((int*)(argv[2]));
			auto win = *((int*)(argv[3]));

			Init(srcx, srcy);

			Kuwahara(Input, srcx, srcy, win);
		}
	}
}
