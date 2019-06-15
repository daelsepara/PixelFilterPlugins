#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "includes/FilterCommon.h"

extern "C"
{
	static unsigned char* ScaledImage = NULL;

	const int FilterID = 0x5D26;
	const char* FilterName = "OmniScale";
	const char* FilterDescription = "Lior Halphon's Omniscale (Modified: Uses Maxim Stepin's Color comparison routine)";

	bool ComparisonThreshold = true;
	int FilterScaleX = 2;
	int FilterScaleY = 2;

	#include "includes/Init.h"
	
	bool is_different(int a, int b)
	{
        return IsNotLike(a, b);
    }
    
    int Interpolate2P1Q(int pixel1, int pixel2, double quantifier)
    {
        auto r = Red(pixel1) * (1.0 - quantifier) + Red(pixel2) * quantifier;
        auto g = Green(pixel1) * (1.0 - quantifier) + Green(pixel2) * quantifier;
        auto b = Blue(pixel1) * (1.0 - quantifier) + Blue(pixel2) * quantifier;
    
        return RGBINT(r, g, b);
    }
    
    int mix(int x, int y, double a)
    {
        return Interpolate2P1Q(x, y, a);
    }
    
    double fract(double x)
    {
        return x - std::floor(x);
    }    

    bool P(int pattern, int m, int r)
    {

        return ((pattern & (m)) == (r));
    }

    int Mul(int x, double y)
    {
        auto r = (unsigned char)(Red(x) * y);
        auto g = (unsigned char)(Green(x) * y);
        auto b = (unsigned char)(Blue(x) * y);

        return RGBINT(r, g, b);
    }
    
    int Add(int x, int y, double scale)
    {

        auto r = (Red(x) + Red(y)) * scale;
        auto g = (Green(x) + Green(y)) * scale;
        auto b = (Blue(x) + Blue(y)) * scale;

        return RGBINT(r, g, b);
    }
    
	double length(double a, double b) {

		return std::sqrt(a * a + b * b);
	}
        
    double ScaleImage(unsigned char*& image, double ppx, double ppy, int srcx, int srcy, int dstx, int dsty)
    {
        auto ox = 1.0 / srcx;
        auto oy = 1.0 / srcy;

        auto px = fract(ppx * (double)srcx);
        auto py = fract(ppy * (double)srcy);

        if (px > 0.5) {
            
            ox = -ox;
            px = 1.0 - px;
        }
        
        if (py > 0.5) {
            
            oy = -oy;
            py = 1.0 - py;
        }

        // convert texture coordinates to image coordinates
        ox = (int)(ox * srcx);
        oy = (int)(oy * srcy);

        auto positionx = (int)(ppx * (double)srcx);
        auto positiony = (int)(ppy * (double)srcy);
		
        auto w0 = CLR(image, srcx, srcy, positionx, positiony, -ox, -oy);
        auto w1 = CLR(image, srcx, srcy, positionx, positiony, 0, -oy);
        auto w2 = CLR(image, srcx, srcy, positionx, positiony, ox, -oy);
        auto w3 = CLR(image, srcx, srcy, positionx, positiony, -ox, 0);
        auto w4 = CLR(image, srcx, srcy, positionx, positiony, 0,  0);
        auto w5 = CLR(image, srcx, srcy, positionx, positiony, ox, 0);
        auto w6 = CLR(image, srcx, srcy, positionx, positiony, -ox, oy);
        auto w7 = CLR(image, srcx, srcy, positionx, positiony, 0,  oy);
        auto w8 = CLR(image, srcx, srcy, positionx, positiony, ox, oy);

        int pattern = 0;
        
        if (is_different(w0, w4)) pattern |= (1 << 0);
        if (is_different(w1, w4)) pattern |= (1 << 1);
        if (is_different(w2, w4)) pattern |= (1 << 2);
        if (is_different(w3, w4)) pattern |= (1 << 3);
        if (is_different(w5, w4)) pattern |= (1 << 4);
        if (is_different(w6, w4)) pattern |= (1 << 5);
        if (is_different(w7, w4)) pattern |= (1 << 6);
        if (is_different(w8, w4)) pattern |= (1 << 7);

        if ((P(pattern, 0xbf, 0x37) || P(pattern, 0xdb, 0x13)) && is_different(w1, w5))
            return mix(w4, w3, 0.5 - px);
        
        if ((P(pattern, 0xdb, 0x49) || P(pattern, 0xef, 0x6d)) && is_different(w7, w3))
            return mix(w4, w1, 0.5 - py);
        
        if ((P(pattern, 0x0b, 0x0b) || P(pattern, 0xfe, 0x4a) || P(pattern, 0xfe, 0x1a)) && is_different(w3, w1))
            return w4;
        
        if ((P(pattern, 0x6f, 0x2a) || P(pattern, 0x5b, 0x0a) || P(pattern, 0xbf, 0x3a) || P(pattern, 0xdf, 0x5a) || P(pattern, 0x9f, 0x8a) || P(pattern, 0xcf, 0x8a) || P(pattern, 0xef, 0x4e) || P(pattern, 0x3f, 0x0e) ||
            P(pattern, 0xfb, 0x5a) || P(pattern, 0xbb, 0x8a) || P(pattern, 0x7f, 0x5a) || P(pattern, 0xaf, 0x8a) || P(pattern, 0xeb, 0x8a)) && is_different(w3, w1))
            return mix(w4, mix(w4, w0, 0.5 - px), 0.5 - py);
        
        if (P(pattern, 0x0b, 0x08))
            return mix(mix(Mul(w0, 0.375) + Mul(w1, 0.25) + Mul(w4, 0.375), Mul(w4, 0.5) + Mul(w1, 0.5), px * 2.0), w4, py * 2.0);

        if (P(pattern, 0x0b, 0x02))
            return mix(mix(Mul(w0, 0.375) + Mul(w3, 0.25) + Mul(w4, 0.375), Mul(w4, 0.5) + Mul(w3, 0.5), py * 2.0), w4, px * 2.0);
        
        int r;
        double dist, pixel_size;
        
        if (P(pattern, 0x2f, 0x2f)) {

            dist = length(px - 0.5, py - 0.5);
            pixel_size = length(1.0 / (dstx / srcx), 1.0 / (dsty / srcy));
            
            if (dist < 0.5 - pixel_size / 2) {
                
                return w4;
            }

            if (is_different(w0, w1) || is_different(w0, w3)) {
                
                r = mix(w1, w3, py - px + 0.5);
            
            } else {

                r = mix(mix(Mul(w1, 0.375) + Mul(w0, 0.25) + Mul(w3, 0.375), w3, py * 2.0), w1, px * 2.0);
            }

            if (dist > 0.5 + pixel_size / 2) {

                return r;
            }

            return mix(w4, r, (dist - 0.5 + pixel_size / 2) / pixel_size);
        }

        if (P(pattern, 0xbf, 0x37) || P(pattern, 0xdb, 0x13)) {
            
            dist = px - 2.0 * py;
            pixel_size = length(1.0 / (dstx / srcx), 1.0 / (dsty / srcy)) * std::sqrt(5.0);
            
            if (dist > pixel_size / 2) {

                return w1;
            }

            r = mix(w3, w4, px + 0.5);

            if (dist < -pixel_size / 2) {
                
                return r;
            }

            return mix(r, w1, (dist + pixel_size / 2) / pixel_size);
        }

        if (P(pattern, 0xdb, 0x49) || P(pattern, 0xef, 0x6d)) {

            dist = py - 2.0 * px;
            pixel_size = length(1.0 / (dstx / srcx), 1.0 / (dsty / srcy)) * std::sqrt(5.0);
            
            if (py - 2.0 * px > pixel_size / 2) {
                
                return w3;
            }

            r = mix(w1, w4, px + 0.5);

            if (dist < -pixel_size / 2) {
                
                return r;
            }
            
            return mix(r, w3, (dist + pixel_size / 2) / pixel_size);
        }

        if (P(pattern, 0xbf,0x8f) || P(pattern, 0x7e,0x0e)) {

            dist = px + 2.0 * py;

            pixel_size = length(1.0 / (dstx / srcx), 1.0 / (dsty / srcy)) * std::sqrt(5.0);
    
            if (dist > 1.0 + pixel_size / 2) {
                
                return w4;
            }
    
            if (is_different(w0, w1) || is_different(w0, w3)) {
                
                r = mix(w1, w3, py - px + 0.5);
            
            } else {
                
                r = mix(mix(Mul(w1, 0.375) + Mul(w0, 0.25) + Mul(w3, 0.375), w3, py * 2.0), w1, px * 2.0);
            }
    
            if (dist < 1.0 - pixel_size / 2) {

                return r;
            }
    
            return mix(r, w4, (dist + pixel_size / 2 - 1.0) / pixel_size);    
        }

        if (P(pattern, 0x7e, 0x2a) || P(pattern, 0xef, 0xab)) {

            dist = py + 2.0 * px;
            pixel_size = length(1.0 / (dstx / srcx), 1.0 / (dsty / srcy)) * std::sqrt(5.0);
    
            if (py + 2.0 * px > 1.0 + pixel_size / 2) {
                
                return w4;
            }
    
            if (is_different(w0, w1) || is_different(w0, w3)) {

                r = mix(w1, w3, py - px + 0.5);
            
            } else {

                r = mix(mix(Mul(w1, 0.375) + Mul(w0, 0.25) + Mul(w3, 0.375), w3, py * 2.0), w1, px * 2.0);
            }
    
            if (dist < 1.0 - pixel_size / 2) {

                return r;
            }
    
            return mix(r, w4, (dist + pixel_size / 2 - 1.0) / pixel_size);
        }
    
        if (P(pattern, 0x1b, 0x03) || P(pattern, 0x4f, 0x43) || P(pattern, 0x8b, 0x83) || P(pattern, 0x6b, 0x43))
            return mix(w4, w3, 0.5 - px);
    
        if (P(pattern, 0x4b, 0x09) || P(pattern, 0x8b, 0x89) || P(pattern, 0x1f, 0x19) || P(pattern, 0x3b, 0x19))
            return mix(w4, w1, 0.5 - py);
    
        if (P(pattern, 0xfb,0x6a) || P(pattern, 0x6f,0x6e) || P(pattern, 0x3f,0x3e) || P(pattern, 0xfb,0xfa) || P(pattern, 0xdf,0xde) || P(pattern, 0xdf,0x1e))
            return mix(w4, w0, (1.0 - px - py) / 2.0);
    
        if (P(pattern, 0x4f,0x4b) || P(pattern, 0x9f,0x1b) || P(pattern, 0x2f,0x0b) || P(pattern, 0xbe,0x0a) || P(pattern, 0xee,0x0a) || P(pattern, 0x7e,0x0a) || P(pattern, 0xeb,0x4b) || P(pattern, 0x3b,0x1b)) {

            dist = px + py;
            pixel_size = length(1.0 / (dstx / srcx), 1.0 / (dsty / srcy));
    
            if (dist > 0.5 + pixel_size / 2) {

                return w4;
            }
    
            if (is_different(w0, w1) || is_different(w0, w3)) {
                
                r = mix(w1, w3, py - px + 0.5);
            
            } else {

                r = mix(mix(Mul(w1, 0.375) + Mul(w0, 0.25) + Mul(w3, 0.375), w3, py * 2.0), w1, px * 2.0);
            }
    
            if (dist < 0.5 - pixel_size / 2) {

                return r;
            }
    
            return mix(r, w4, (dist + pixel_size / 2 - 0.5) / pixel_size);
        }
    
        if (P(pattern, 0x0b, 0x01))
            return mix(mix(w4, w3, 0.5 - px), mix(w1, Add(w1,  w3, 0.5), 0.5 - px), 0.5 - py);
    
        if (P(pattern, 0x0b, 0x00))
            return mix(mix(w4, w3, 0.5 - px), mix(w1, w0, 0.5 - px), 0.5 - py);
    
        dist = px + py;
        pixel_size = length(1.0 / (dstx / srcx), 1.0 / (dsty / srcy));
    
        if (dist > 0.5 + pixel_size / 2)
            return w4;

        /* We need more samples to "solve" this diagonal */
        auto x0 = CLR(image, srcx, srcy, positionx, positiony, -ox * 2, -oy);
        auto x1 = CLR(image, srcx, srcy, positionx, positiony, -ox, -oy * 2);
        auto x2 = CLR(image, srcx, srcy, positionx, positiony, 0, -oy * 2);
        auto x3 = CLR(image, srcx, srcy, positionx, positiony, ox, -oy * 2);
        auto x4 = CLR(image, srcx, srcy, positionx, positiony, -ox * 2, -oy);
        auto x5 = CLR(image, srcx, srcy, positionx, positiony, -ox * 2,  0);
        auto x6 = CLR(image, srcx, srcy, positionx, positiony, -ox * 2,  oy);

        if (is_different(x0, w4)) pattern |= 1 << 8;
        if (is_different(x1, w4)) pattern |= 1 << 9;
        if (is_different(x2, w4)) pattern |= 1 << 10;
        if (is_different(x3, w4)) pattern |= 1 << 11;
        if (is_different(x4, w4)) pattern |= 1 << 12;
        if (is_different(x5, w4)) pattern |= 1 << 13;
        if (is_different(x6, w4)) pattern |= 1 << 14;

        auto diagonal_bias = -7;
        
        while (pattern != 0) {
            
            diagonal_bias += pattern & 1;
            pattern >>= 1;
        }

        if (diagonal_bias <=  0) {
            
            r = mix(w1, w3, py - px + 0.5);

            if (dist < 0.5 - pixel_size / 2) {
                
                return r;
            }
            
            return mix(r, w4, (dist + pixel_size / 2 - 0.5) / pixel_size);
        }

        return w4;            
    }
    
	void Apply(int argc, void** argv)
	{
		if (argc >= 4)
		{
			auto Input = ((unsigned char*)(argv[0]));
			auto srcx = *((int*)(argv[1]));
			auto srcy = *((int*)(argv[2]));
			auto scale = *((int*)(argv[3]));

			auto Channels = 3;
			
			scale = std::max(1, scale);
			
			FilterScaleX = scale;
			FilterScaleY = scale;
			
			Init(srcx, srcy);
			
			for (auto y = 0; y < _SizeY; y++) {

				auto offset = y * _SizeX;
				double positiony = ((double)y / _SizeY);

				for (auto x = 0; x < _SizeX; x++) {

					auto rgb = ScaleImage(Input, ((double)x / _SizeX), positiony, srcx, srcy, _SizeX, _SizeY);
					
					ScaledImage[(offset + x) * Channels] = Red(rgb);
					ScaledImage[(offset + x) * Channels + 1] = Green(rgb);
					ScaledImage[(offset + x) * Channels + 2] = Blue(rgb);
				}
			}
		}
	}
}
