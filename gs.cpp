#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fftw3.h>

#include "includes/FilterCommon.h"

extern "C"
{
	static unsigned char* ScaledImage = NULL;

	const int FilterID = 0x5D25;
	const char* FilterName = "Gerchberg-Saxton";
	const char* FilterDescription = "Generate Fourier Hologram";

	bool ComparisonThreshold = false;
	const int FilterScaleX = 1;
	const int FilterScaleY = 1;

	const int Re = 0;
	const int Im = 1;
	
	#include "includes/Init.h"

	double* Double(int Length, double val)
	{
		auto dbl = (double*)malloc(Length * sizeof(double));

		if (dbl != NULL)
		{
			for (auto x = 0; x < Length; x++)
			{
				dbl[x] = val;
			}
		}

		return dbl;
	}

	fftw_complex* Complex(int Length, double val)
	{
		auto dbl = (fftw_complex*)fftw_malloc(Length * sizeof(fftw_complex));

		if (dbl != NULL)
		{
			for (auto x = 0; x < Length; x++)
			{
				dbl[x][Re] = val;
				dbl[x][Im] = 0.0;
			}
		}

		return dbl;
	}
	
	fftw_complex* Copy(unsigned char* input, int xdim, int ydim)
	{
		auto cmplx = (fftw_complex*)fftw_malloc(xdim * ydim * sizeof(fftw_complex));

		if (cmplx != NULL)
		{
			for (auto y = 0; y < ydim; y++)
			{
				for (auto x = 0; x < xdim; x++)
				{
					auto index = y * xdim + x;
					
					cmplx[index][Re] = Luminance(CLR(input, xdim, ydim, x, y));
					cmplx[index][Im] = Luminance(CLR(input, xdim, ydim, x, y));
				}
			}
		}

		return cmplx;
	}
	
	void Source(fftw_complex** source, int xdim, int ydim)
	{
		auto size = xdim * ydim;

		*source = Complex(size, 0.0);

		if (source)
		{
			// create source field		
			for (auto index = 0; index < size; index++)
			{
				(*source)[index][Re] = 1.0;
			}
		}
	}

	void Shift(fftw_complex* complex, int sizex, int sizey)
	{
		fftw_complex temp;
		auto midx = sizex / 2;
		auto midy = sizey / 2;

		for (auto y = 0; y < midy; y++)
		{
			for (auto x = 0; x < midx; x++)
			{
				// Exchange 1st and 4th quadrant
				temp[Re] = complex[y * sizex + x][Re];
				complex[y * sizex + x][Re] = complex[(midy + y) * sizex + (midx + x)][Re];
				complex[(midy + y) * sizex + (midx + x)][Re] = temp[Re];

				temp[Im] = complex[y * sizex + x][Im];
				complex[y * sizex + x][Im] = complex[(midy + y) * sizex + (midx + x)][Im];
				complex[(midy + y) * sizex + (midx + x)][Im] = temp[Im];

				// Exchange 2nd and 3rd quadrant
				temp[Re] = complex[y * sizex + (midx + x)][Re];
				complex[y * sizex + (midx + x)][Re] = complex[(midy + y) * sizex + x][Re];
				complex[(midy + y) * sizex + x][Re] = temp[Re];

				temp[Im] = complex[y * sizex + (midx + x)][Im];
				complex[y * sizex + (midx + x)][Im] = complex[(midy + y) * sizex + x][Im];
				complex[(midy + y) * sizex + x][Im] = temp[Im];
			}
		}
	}

	double Rsqrt(double x)
	{
		return 1.0 / sqrt(x);
	}
	
	void ComputePhase(fftw_complex* input, fftw_complex* phase, int size)
	{
		for (auto index = 0; index < size; index++)
		{
			auto pf = Rsqrt(input[index][Re] * input[index][Re] + input[index][Im] * input[index][Im]);

			phase[index][Re] = input[index][Re] * pf;
			phase[index][Im] = input[index][Im] * pf;
		}
	}

	void ComputePhaseAndMakeComplexField(fftw_complex* input, fftw_complex* amplitude, fftw_complex* ComplexField, int size)
	{
		for (int index = 0; index < size; index++)
		{
			auto pf = Rsqrt(input[index][Re] * input[index][Re] + input[index][Im] * input[index][Im]);

			auto phase_re = input[index][Re] * pf;
			auto phase_im = input[index][Im] * pf;

			ComplexField[index][Re] = amplitude[index][Re] * phase_re - amplitude[index][Im] * phase_im;
			ComplexField[index][Im] = amplitude[index][Re] * phase_im + amplitude[index][Im] * phase_re;
		}
	}
	
	double Mod(double a, double m)
	{
		return a - m * floor(a / m);
	}
		
	void Apply(int argc, void** argv)
	{
		if (argc >= 4)
		{
			auto Input = ((unsigned char*)(argv[0]));
			auto srcx = *((int*)(argv[1]));
			auto srcy = *((int*)(argv[2]));
			auto Ngs = *((int*)(argv[3]));

			Ngs = Ngs < 1 ? 1 : Ngs;
			Ngs = Ngs > 1000 ? 1000 : Ngs;
			
			Init(srcx, srcy);
			
			auto Channels = 3;
			
			// FFT plans
			fftw_plan fwdplan, invplan;
			
			auto size = srcx * srcy;

			// temporary complex arrays to hold intermediate results
			auto result = Complex(size, 0.0);
			auto source = Complex(size, 0.0);
			auto d_temp = Complex(size, 0.0);
			auto d_result = Complex(size, 0.0);
			
			Source(&source, srcx, srcy);

			// 2D Forward plan
			fwdplan = fftw_plan_dft_2d(srcy, srcx, d_temp, d_result, FFTW_FORWARD, FFTW_MEASURE);

			// 2D Inverse plan
			invplan = fftw_plan_dft_2d(srcy, srcx, d_temp, d_result, FFTW_BACKWARD, FFTW_MEASURE);

			auto resized = Copy(Input, srcx, srcy);
			
			Shift(resized, srcx, srcy);
			
			// ---------------------
			// --    GS PHASE     --
			// ---------------------

			// Get initial estimate of the phase
			fftw_execute_dft(invplan, resized, d_result);

			for (auto iter = 0; iter < Ngs; iter++)
			{
				// Apply source constraints
				ComputePhaseAndMakeComplexField(d_result, source, d_temp, size);

				// perform forward transform
				fftw_execute_dft(fwdplan, d_temp, d_result);

				// Apply target constraints
				ComputePhaseAndMakeComplexField(d_result, resized, d_temp, size);

				// perform backward transform
				fftw_execute_dft(invplan, d_temp, d_result);
			}

			// ----- extract GS PHASE
			ComputePhase(d_result, result, size);

			for (auto index = 0; index < size; index++)
			{
				auto angle = Mod(atan2(result[index][Im], result[index][Re]), 2.0 * M_PI);

				unsigned char c = (unsigned char)(255.0 * angle / (2.0 * M_PI));
				
				ScaledImage[index * Channels] = c;
				ScaledImage[index * Channels + 1] = c;
				ScaledImage[index * Channels + 2] = c;
			}

			// Destroy plans
			fftw_destroy_plan(invplan);
			fftw_destroy_plan(fwdplan);

			// FFTW clean-up
			fftw_cleanup();

			// free memory
			fftw_free(resized);
			fftw_free(d_result);
			fftw_free(d_temp);
			fftw_free(result);
			fftw_free(source);
		}
	}
}
