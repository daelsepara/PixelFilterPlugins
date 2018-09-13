#define _USE_MATH_DEFINES
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <limits>
#include <stdexcept>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "lodepng.h"
#include "Parameters.hpp"
#include "PixelFilter.hpp"
#include "Profiler.hpp"

unsigned char* loadpng(char* filename, int* xdim, int* ydim)
{
	unsigned error;
	unsigned char* image;
	unsigned w, h;

	// load PNG
	error = lodepng_decode24_file(&image, &w, &h, filename);

	// exit on error
	if (error)
	{
		fprintf(stderr, "decoder error %u: %s\n", error, lodepng_error_text(error));

		exit(1);
	}

	*xdim = w;
	*ydim = h;

	fprintf(stderr, "Image: %s loaded, size = (%d x %d)\n", filename, w, h);

	return image;
}

unsigned char* loadimage(char* filename, int* xdim, int* ydim)
{
	auto Channels = 3;

	auto img = cv::imread(filename, cv::IMREAD_COLOR);

	if (img.empty()) // Check for invalid input
	{
		fprintf(stderr, "Error on loading image: %s\n", filename);

		exit(1);
	}

	fprintf(stderr, "loaded %d x %d image\n", img.cols, img.rows);

	auto *image = (unsigned char*)malloc(img.cols * img.rows * Channels);

	for (auto y = 0; y < img.rows; y++)
	{
		for (auto x = 0; x < img.cols; x++)
		{
			auto b = img.at<cv::Vec3b>(y, x)[0];
			auto g = img.at<cv::Vec3b>(y, x)[1];
			auto r = img.at<cv::Vec3b>(y, x)[2];

			image[(y * img.cols + x) * Channels] = r;
			image[(y * img.cols + x) * Channels + 1] = g;
			image[(y * img.cols + x) * Channels + 2] = b;
		}
	}

	*xdim = img.cols;
	*ydim = img.rows;

	img.release();

	return image;
}

void savepng(const char* filename, unsigned char*& buffer, int xdim, int ydim)
{
	auto error = lodepng_encode24_file(filename, buffer, xdim, ydim);

	if (error)
	{
		fprintf(stderr, "error %u: %s\n", error, lodepng_error_text(error));
	}
}

void ParseInt(std::string arg, const char* str, const char* var, int& dst)
{
	auto len = strlen(str);

	if (len > 0)
	{
		if (!arg.compare(0, len, str) && arg.length() > len)
		{
			try
			{
				auto val = stoi(arg.substr(len));

				fprintf(stderr, "... %s = %d\n", var, val);

				dst = val;
			}
			catch (const std::invalid_argument& ia)
			{
				fprintf(stderr, "... %s = NaN %s\n", var, ia.what());
				exit(1);
			}
		}
	}
}

void ParseBool(std::string arg, const char* str, const char* var, bool& dst)
{
	auto len = strlen(str);

	if (len > 0)
	{
		if (!arg.compare(0, len, str) && arg.length() > len)
		{
			try
			{
				auto val = stoi(arg.substr(len));

				fprintf(stderr, "... %s = %s\n", var, val > 0 ? "on" : "off");

				dst = val > 0;
			}
			catch (const std::invalid_argument& ia)
			{
				fprintf(stderr, "... %s = Nan %s\n", var, ia.what());

				exit(1);
			}
		}
	}
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "To use:\n\n%s [parameters...]\n", argv[0]);

		exit(1);
	}

	Parameters parameters;

	char InputFile[200];
	char OutputFile[200];

	InputFile[0] = '\0';
	OutputFile[0] = '\0';

	for (int i = 0; i < argc; i++)
	{
		std::string arg = argv[i];
		std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);

		if (!arg.compare("/help"))
		{
			fprintf(stderr, "\nAvailable parameters:\n");
			fprintf(stderr, "=====================\n\n");
			fprintf(stderr, "/output=[output.png] specify output image png file\n\n");
			fprintf(stderr, "/input=[input.png] specify input image png file\n\n");
			fprintf(stderr, "/filters show available filters/filters\n\n");
			fprintf(stderr, "/filter=[specify type of pixel scaler/filter to apply, e.g. /filter=epx\n\n");
			fprintf(stderr, "/magnification=[magnification, e.g. 2] specify magnification\n\n");
			fprintf(stderr, "/threshold=[0, 1 default: 0] enable / disable threshold when comparing pixels\n\n");
			exit(0);
		}

		if (!arg.compare("/filters"))
		{
			fprintf(stderr, "\nAvailable Filters:\n");
			fprintf(stderr, "=====================\n\n");
			fprintf(stderr, "bilinear, magnify, nearest\n");
			fprintf(stderr, "epx, epxb, epxc, epx3\n");
			fprintf(stderr, "scale2x, scale3x\n");
			fprintf(stderr, "amscale2x, amscale3x\n");
			fprintf(stderr, "advinterp2x, advinterp3x\n");
			fprintf(stderr, "eagle2x, eagle3x, eagle3xb, supereagle\n");
			fprintf(stderr, "2xsai, supersai, reverseaa\n");
			fprintf(stderr, "des, des2x, super2x, ultra2x, 2xscl\n");
			fprintf(stderr, "hq2x, hq3x, hq4x\n");
			fprintf(stderr, "hq2x2, hq2x3, hq2x4\n");
			fprintf(stderr, "lq2x, lq3x, lq4x\n");
			fprintf(stderr, "lq2x2, lq2x3, lq2x4\n");
			fprintf(stderr, "horiz2x, horiz3x, horiz4x\n");
			fprintf(stderr, "vertscan2x, vertscan3x, vertscan4x\n");
			fprintf(stderr, "tv2x, tv3x, tv4x\n");
			fprintf(stderr, "rgb2x, rgb3x, rgb4x\n");
			fprintf(stderr, "xbr2x, xbr3x, xbr4x\n");
			fprintf(stderr, "xbrz2x, xbrz3x, xbrz4x, xbrz5x, xbrz6x\n");
			fprintf(stderr, "fliph, flipv\n");
			fprintf(stderr, "rot90, rot180, rot270\n");
			fprintf(stderr, "kuwahara, kuwahara7, kuwahara9, kuwahara11\n");
			fprintf(stderr, "\n");

			exit(0);
		}

		if (!arg.compare(0, 8, "/filter=") && arg.length() > 8)
		{
			auto type = arg.substr(8);

			if (!type.compare("epx") || !type.compare("scale2x"))
			{
				fprintf(stderr, "Filter: EPX / Scale2X\n");
				parameters.EPX = true;
				parameters.Magnification = 2;
			}

			if (!type.compare("epx3"))
			{
				fprintf(stderr, "Filter: EPX 3X\n");
				parameters.EPX = true;
				parameters.Magnification = 3;
			}
			
			if (!type.compare("epxb"))
			{
				fprintf(stderr, "Filter: EPX ver. B\n");
				parameters.EPXB = true;
			}

			if (!type.compare("epxc"))
			{
				fprintf(stderr, "Filter: EPX ver. C\n");
				parameters.EPXC = true;
			}

			if (!type.compare("scale3x"))
			{
				fprintf(stderr, "Filter: Scale3X\n");
				parameters.Scale3X = true;
			}

			if (!type.compare("magnify"))
			{
				fprintf(stderr, "Filter: Magnify\n");
				parameters.Magnify = true;
			}

			if (!type.compare("nearest"))
			{
				fprintf(stderr, "Filter: Nearest-neighbor\n");
				parameters.Nearest = true;
			}

			if (!type.compare("bilinear"))
			{
				fprintf(stderr, "Filter: Bilinear\n");
				parameters.Bilinear = true;
			}
			
			if (!type.compare("bilinearplus"))
			{
				fprintf(stderr, "Filter: Bilinear+\n");
				parameters.BilinearPlus = true;
			}

			if (!type.compare("bilinearpp"))
			{
				fprintf(stderr, "Filter: Bilinear++\n");
				parameters.BilinearPP = true;
			}
			
			if (!type.compare("lq2x"))
			{
				fprintf(stderr, "Filter: LQ2X\n");
				parameters.LQX = true;
				parameters.Magnification = 2;
			}

			if (!type.compare("lq3x"))
			{
				fprintf(stderr, "Filter: LQ3X\n");
				parameters.LQX = true;
				parameters.Magnification = 3;
			}

			if (!type.compare("lq4x"))
			{
				fprintf(stderr, "Filter: LQ4X\n");
				parameters.LQX = true;
				parameters.Magnification = 4;
			}

			if (!type.compare("lq2x2"))
			{
				fprintf(stderr, "Filter: LQ2XN (2x2)\n");
				parameters.LQ2XN = true;
				parameters.Magnification = 2;
			}
			
			if (!type.compare("lq2x3"))
			{
				fprintf(stderr, "Filter: LQ2XN (2x3)\n");
				parameters.LQ2XN = true;
				parameters.Magnification = 3;
			}
			
			if (!type.compare("lq2x4"))
			{
				fprintf(stderr, "Filter: LQ2XN (2x4)\n");
				parameters.LQ2XN = true;
				parameters.Magnification = 4;
			}
			
			if (!type.compare("eagle2x"))
			{
				fprintf(stderr, "Filter: Eagle2X\n");
				parameters.Eagle = true;
				parameters.Magnification = 2;
			}

			if (!type.compare("eagle3x"))
			{
				fprintf(stderr, "Filter: Eagle3X\n");
				parameters.Eagle = true;
				parameters.Magnification = 3;
			}

			if (!type.compare("eagle3xb"))
			{
				fprintf(stderr, "Filter: Eagle3XB\n");
				parameters.Eagle3XB = true;
			}

			if (!type.compare("supereagle"))
			{
				fprintf(stderr, "Filter: Super Eagle (2X)\n");
				parameters.SuperEagle = true;
			}

			if (!type.compare("super2x"))
			{
				fprintf(stderr, "Filter: Super2X\n");
				parameters.Super2X = true;
			}

			if (!type.compare("ultra2x"))
			{
				fprintf(stderr, "Filter: Ultra2X\n");
				parameters.Ultra2X = true;
			}
			
			if (!type.compare("des"))
			{
				fprintf(stderr, "Filter: DES\n");
				parameters.DES = true;
			}
			
			if (!type.compare("des2x"))
			{
				fprintf(stderr, "Filter: DES2X\n");
				parameters.DES2X = true;
			}

			if (!type.compare("tv2x"))
			{
				fprintf(stderr, "Filter: TV2X\n");
				parameters.TV = true;
				parameters.Magnification = 2;
			}

			if (!type.compare("tv3x"))
			{
				fprintf(stderr, "Filter: TV3X\n");
				parameters.TV = true;
				parameters.Magnification = 3;
			}
			
			if (!type.compare("tv4x"))
			{
				fprintf(stderr, "Filter: TV4X\n");
				parameters.TV = true;
				parameters.Magnification = 4;
			}

			if (!type.compare("rgb2x"))
			{
				fprintf(stderr, "Filter: RGB2X\n");
				parameters.RGB = true;
				parameters.Magnification = 2;
			}

			if (!type.compare("rgb3x"))
			{
				fprintf(stderr, "Filter: RGB3X\n");
				parameters.RGB = true;
				parameters.Magnification = 3;
			}
			
			if (!type.compare("rgb4x"))
			{
				fprintf(stderr, "Filter: RGB4X\n");
				parameters.RGB = true;
				parameters.Magnification = 4;
			}

			if (!type.compare("horiz2x"))
			{
				fprintf(stderr, "Filter: Horizontal Scanlines (2X)\n");
				parameters.Horiz = true;
				parameters.Magnification = 2;
			}

			if (!type.compare("horiz3x"))
			{
				fprintf(stderr, "Filter: Horizontal Scanlines (3X)\n");
				parameters.Horiz = true;
				parameters.Magnification = 3;
			}
			
			if (!type.compare("horiz4x"))
			{
				fprintf(stderr, "Filter: Horizontal Scanlines (4X)\n");
				parameters.Horiz = true;
				parameters.Magnification = 4;
			}

			if (!type.compare("advinterp2x"))
			{
				fprintf(stderr, "Filter: AdvanceMame Interpolation (2X)\n");
				parameters.AdvInterp = true;
				parameters.Magnification = 2;
			}

			if (!type.compare("advinterp3x"))
			{
				fprintf(stderr, "Filter: AdvanceMame Interpolation (3X)\n");
				parameters.AdvInterp = true;
				parameters.Magnification = 3;
			}
			
			if (!type.compare("amscale2x"))
			{
				fprintf(stderr, "Filter: Andrea Mazzoleni's Scale2X modified by Hawkynt to support thresholds\n");
				parameters.AmScale = true;
				parameters.Magnification = 2;
			}

			if (!type.compare("amscale3x"))
			{
				fprintf(stderr, "Filter: Andrea Mazzoleni's Scale3X modified by Hawkynt to support thresholds\n");
				parameters.AmScale = true;
				parameters.Magnification = 3;
			}

			if (!type.compare("vertscan2x"))
			{
				fprintf(stderr, "Filter: Vertical Scanlines (2X)\n");
				parameters.VertScan = true;
				parameters.Magnification = 2;
			}

			if (!type.compare("vertscan3x"))
			{
				fprintf(stderr, "Filter: Vertical Scanlines (3X)\n");
				parameters.VertScan = true;
				parameters.Magnification = 3;
			}

			if (!type.compare("vertscan4x"))
			{
				fprintf(stderr, "Filter: Vertical Scanlines (4X)\n");
				parameters.VertScan = true;
				parameters.Magnification = 4;
			}

			if (!type.compare("2xscl"))
			{
				fprintf(stderr, "Filter: SNES 2XSCL (2X)\n");
				parameters.SCL2X = true;
			}

			if (!type.compare("xbr2x"))
			{
				fprintf(stderr, "Filter: XBR (2X)\n");
				parameters.XBR = true;
				parameters.Magnification = 2;
			}

			if (!type.compare("xbr3x"))
			{
				fprintf(stderr, "Filter: XBR (3X)\n");
				parameters.XBR = true;
				parameters.Magnification = 3;
			}

			if (!type.compare("xbr4x"))
			{
				fprintf(stderr, "Filter: XBR (4X)\n");
				parameters.XBR = true;
				parameters.Magnification = 4;
			}

			if (!type.compare("xbrz2x"))
			{
				fprintf(stderr, "Filter: XBRZ (2X)\n");
				parameters.XBRZ = true;
				parameters.Magnification = 2;
			}

			if (!type.compare("xbrz3x"))
			{
				fprintf(stderr, "Filter: XBRZ (3X)\n");
				parameters.XBRZ = true;
				parameters.Magnification = 3;
			}

			if (!type.compare("xbrz4x"))
			{
				fprintf(stderr, "Filter: XBRZ (4X)\n");
				parameters.XBRZ = true;
				parameters.Magnification = 4;
			}

			if (!type.compare("xbrz5x"))
			{
				fprintf(stderr, "Filter: XBRZ (5X)\n");
				parameters.XBRZ = true;
				parameters.Magnification = 5;
			}

			if (!type.compare("xbrz6x"))
			{
				fprintf(stderr, "Filter: XBRZ (6X)\n");
				parameters.XBRZ = true;
				parameters.Magnification = 6;
			}

			if (!type.compare("hq2x"))
			{
				fprintf(stderr, "Filter: HQnX (2X)\n");
				parameters.HQX = true;
				parameters.Magnification = 2;
			}

			if (!type.compare("hq3x"))
			{
				fprintf(stderr, "Filter: HQnX (3X)\n");
				parameters.HQX = true;
				parameters.Magnification = 3;
			}

			if (!type.compare("hq4x"))
			{
				fprintf(stderr, "Filter: HQnX (4X)\n");
				parameters.HQX = true;
				parameters.Magnification = 4;
			}

			if (!type.compare("hq2x2"))
			{
				fprintf(stderr, "Filter: HQ2XN (2x2)\n");
				parameters.HQ2XN = true;
				parameters.Magnification = 2;
			}
			
			if (!type.compare("hq2x3"))
			{
				fprintf(stderr, "Filter: HQ2XN (2x3)\n");
				parameters.HQ2XN = true;
				parameters.Magnification = 3;
			}
			
			if (!type.compare("hq2x4"))
			{
				fprintf(stderr, "Filter: HQ2XN (2x4)\n");
				parameters.HQ2XN = true;
				parameters.Magnification = 4;
			}
			
			if (!type.compare("2xsai"))
			{
				fprintf(stderr, "Filter: 2X SAI\n");
				parameters.SAI2X = true;
			}

			if (!type.compare("supersai"))
			{
				fprintf(stderr, "Filter: Super SAI (2X)\n");
				parameters.SuperSAI = true;
			}

			if (!type.compare("reverseaa"))
			{
				fprintf(stderr, "Filter: Reverse Anti-Aliasing (2X)\n");
				parameters.ReverseAA = true;
			}
			
			if (!type.compare("fliph"))
			{
				fprintf(stderr, "Filter: Flip horizontally\n");
				parameters.Flip = true;
				parameters.FlipDir = parameters.FlipH;
			}
			
			if (!type.compare("flipv"))
			{
				fprintf(stderr, "Filter: Flip vertically\n");
				parameters.Flip = true;
				parameters.FlipDir = parameters.FlipV;
			}
			
			if (!type.compare("rot90"))
			{
				fprintf(stderr, "Filter: Rotate counter clockwise 90 degrees\n");
				parameters.Rotate = true;
				parameters.RotAngle = parameters.Rot90;
			}
			
			if (!type.compare("rot180"))
			{
				fprintf(stderr, "Filter: Rotate counter clockwise 180 degrees\n");
				parameters.Rotate = true;
				parameters.RotAngle = parameters.Rot180;
			}

			if (!type.compare("rot270"))
			{
				fprintf(stderr, "Filter: Rotate counter clockwise 270 degrees\n");
				parameters.Rotate = true;
				parameters.RotAngle = parameters.Rot270;
			}
			
			if (!type.compare("kuwahara"))
			{
				fprintf(stderr, "Filtering: Kuwahara Smoothing and Edge-preserving filter (1X)\n");
				parameters.Kuwahara = true;
				parameters.Window = 5;
			}

			if (!type.compare("kuwahara7"))
			{
				fprintf(stderr, "Filtering: Kuwahara Smoothing and Edge-preserving filter (1X) using a 7x7 window\n");
				parameters.Kuwahara = true;
				parameters.Window = 7;
			}

			if (!type.compare("kuwahara9"))
			{
				fprintf(stderr, "Filtering: Kuwahara Smoothing and Edge-preserving filter (1X) using a 9x9 window\n");
				parameters.Kuwahara = true;
				parameters.Window = 9;
			}

			if (!type.compare("kuwahara11"))
			{
				fprintf(stderr, "Filtering: Kuwahara Smoothing and Edge-preserving filter (1X) using a 11x11 window\n");
				parameters.Kuwahara = true;
				parameters.Window = 11;
			}
		}

		ParseInt(arg, "/magnification=", "Magnification", parameters.Magnification);
		ParseBool(arg, "/threshold=", "Threshold", parameters.Threshold);

		if (!arg.compare(0, 7, "/input=") && arg.length() > 7)
		{
			strncpy(InputFile, &argv[i][7], sizeof(InputFile));

			fprintf(stderr, "Input PNG File: %s\n", InputFile);
		}

		if (!arg.compare(0, 8, "/output=") && arg.length() > 8)
		{
			strncpy(OutputFile, &argv[i][8], sizeof(OutputFile));

			fprintf(stderr, "Output PNG File: %s\n", OutputFile);
		}
	}

	PixelFilter filter;
	bool Filtered = false;
	
	filter.SetThreshold(parameters.Threshold);

	int srcx = 0;
	int srcy = 0;

	if (strlen(InputFile) < 5)
	{
		fprintf(stderr, "%s: error: no input file\n", argv[0]);
		exit(1);
	}

	auto png = loadimage(InputFile, &srcx, &srcy);

	if (parameters.EPX)
	{
		void* Params[] = { png, &srcx, &srcy, &parameters.Magnification };
		
		filter.Filter("epx", 4, Params);
		
		Filtered = true;
	}
	
	if (parameters.Kuwahara)
	{
		void* Params[] = { png, &srcx, &srcy, &parameters.Window };
		
		filter.Filter("kuwahara", 4, Params);
		
		Filtered = true;
	}

	if (parameters.XBRZ)
	{
		void* Params[] = { png, &srcx, &srcy, &parameters.Magnification };
		
		filter.Filter("xbrz", 4, Params);
		
		Filtered = true;
	}
	
	if (parameters.HQX)
	{
		void* Params[] = { png, &srcx, &srcy, &parameters.Magnification };
		
		filter.Filter("hqx", 4, Params);
		
		Filtered = true;
	}
	
	if (parameters.LQX)
	{
		void* Params[] = { png, &srcx, &srcy, &parameters.Magnification };
		
		filter.Filter("lqx", 4, Params);
		
		Filtered = true;
	}
	
	if (parameters.XBR)
	{
		void* Params[] = { png, &srcx, &srcy, &parameters.Magnification };
		
		filter.Filter("xbr", 4, Params);
		
		Filtered = true;
	}
	
	if (parameters.SAI2X)
	{
		void* Params[] = { png, &srcx, &srcy };
		
		filter.Filter("sai", 3, Params);
		
		Filtered = true;
	}
	
	if (parameters.SuperSAI)
	{
		void* Params[] = { png, &srcx, &srcy };
		
		filter.Filter("supersai", 3, Params);
		
		Filtered = true;
	}
	
	if (parameters.Eagle)
	{
		void* Params[] = { png, &srcx, &srcy, &parameters.Magnification };
		
		filter.Filter("eagle", 4, Params);
		
		Filtered = true;
	}
	
	if (parameters.SuperEagle)
	{
		void* Params[] = { png, &srcx, &srcy };
		
		filter.Filter("supereagle", 3, Params);
		
		Filtered = true;
	}

	if (parameters.RGB)
	{
		void* Params[] = { png, &srcx, &srcy, &parameters.Magnification };
		
		filter.Filter("rgb", 4, Params);
		
		Filtered = true;
	}
	
	if (parameters.TV)
	{
		void* Params[] = { png, &srcx, &srcy, &parameters.Magnification };
		
		filter.Filter("tv", 4, Params);
		
		Filtered = true;
	}

	if (parameters.AdvInterp)
	{
		void* Params[] = { png, &srcx, &srcy, &parameters.Magnification };
		
		filter.Filter("advmameinterp", 4, Params);
		
		Filtered = true;
	}

	if (parameters.Scale3X)
	{
		void* Params[] = { png, &srcx, &srcy };
		
		filter.Filter("scale3x", 3, Params);
		
		Filtered = true;
	}
	
	if (parameters.Magnify)
	{
		void* Params[] = { png, &srcx, &srcy, &parameters.Magnification };
		
		filter.Filter("magnify", 4, Params);
		
		Filtered = true;
	}
	
	if (parameters.AmScale)
	{
		void* Params[] = { png, &srcx, &srcy, &parameters.Magnification };
		
		filter.Filter("amscale", 4, Params);
		
		Filtered = true;
	}
	
	if (parameters.Horiz)
	{
		void* Params[] = { png, &srcx, &srcy, &parameters.Magnification };
		
		filter.Filter("horiz", 4, Params);
		
		Filtered = true;
	}
	
	if (parameters.VertScan)
	{
		void* Params[] = { png, &srcx, &srcy, &parameters.Magnification };
		
		filter.Filter("vertscan", 4, Params);
		
		Filtered = true;
	}
	
	if (parameters.ReverseAA)
	{
		void* Params[] = { png, &srcx, &srcy };
		
		filter.Filter("reverseaa", 3, Params);
		
		Filtered = true;
	}
	
	if (parameters.Ultra2X)
	{
		void* Params[] = { png, &srcx, &srcy };
		
		filter.Filter("ultra2x", 3, Params);
		
		Filtered = true;
	}
	
	if (parameters.Super2X)
	{
		void* Params[] = { png, &srcx, &srcy };
		
		filter.Filter("super2x", 3, Params);
		
		Filtered = true;
	}
	
	if (parameters.SCL2X)
	{
		void* Params[] = { png, &srcx, &srcy };
		
		filter.Filter("2xscl", 3, Params);
		
		Filtered = true;
	}
	
	if (parameters.DES2X)
	{
		void* Params[] = { png, &srcx, &srcy };
		
		filter.Filter("des2x", 3, Params);
		
		Filtered = true;
	}
	
	if (parameters.DES)
	{
		void* Params[] = { png, &srcx, &srcy };
		
		filter.Filter("des", 3, Params);
		
		Filtered = true;
	}
	
	if (parameters.Bilinear)
	{
		void* Params[] = { png, &srcx, &srcy };
		
		filter.Filter("bilinear", 3, Params);
		
		Filtered = true;
	}
	
	if (parameters.BilinearPlus)
	{
		void* Params[] = { png, &srcx, &srcy };
		
		filter.Filter("bilinearplus", 3, Params);
		
		Filtered = true;
	}
	
	if (parameters.BilinearPP)
	{
		void* Params[] = { png, &srcx, &srcy };
		
		filter.Filter("bilinearpp", 3, Params);
		
		Filtered = true;
	}
	
	if (parameters.HQ2XN)
	{
		void* Params[] = { png, &srcx, &srcy, &parameters.Magnification };
		
		filter.Filter("hq2xn", 4, Params);
		
		Filtered = true;
	}
	
	if (parameters.LQ2XN)
	{
		void* Params[] = { png, &srcx, &srcy, &parameters.Magnification };
		
		filter.Filter("lq2xn", 4, Params);
		
		Filtered = true;
	}
	
	if (parameters.EPXB)
	{
		void* Params[] = { png, &srcx, &srcy };
		
		filter.Filter("epxb", 3, Params);
		
		Filtered = true;
	}
	
	if (parameters.EPXC)
	{
		void* Params[] = { png, &srcx, &srcy };
		
		filter.Filter("epxc", 3, Params);
		
		Filtered = true;
	}
	
	if (parameters.Eagle3XB)
	{
		void* Params[] = { png, &srcx, &srcy };
		
		filter.Filter("eagle3xb", 3, Params);
		
		Filtered = true;
	}
	
	if (parameters.Flip)
	{
		void* Params[] = { png, &srcx, &srcy, &parameters.FlipDir };
		
		filter.Filter("flip", 4, Params);
		
		Filtered = true;
	}
	
	if (parameters.Rotate)
	{
		void* Params[] = { png, &srcx, &srcy, &parameters.RotAngle };
		
		filter.Filter("rotate", 4, Params);
		
		Filtered = true;
	}
	
	if (parameters.Nearest)
	{
		void* Params[] = { png, &srcx, &srcy };
		
		filter.Filter("nearest", 3, Params);
		
		Filtered = true;
	}
	
	if (strlen(OutputFile) > 0 && Filtered)
	{
		savepng(OutputFile, filter.ScaledImage, filter.ScaledX, filter.ScaledY);
	}
	
	filter.Free();

	free(png);

	return 0;
}
