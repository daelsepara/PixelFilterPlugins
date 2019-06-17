#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include "includes/FilterCommon.h"

extern "C"
{
	static unsigned char *ScaledImage = NULL;
	const int FilterID = 0x5D26;
	const char *FilterName = "Lanczos Resampling Filter";
	const char *FilterDescription = "Lanczos (window size = 3) resampling filter";

	bool ComparisonThreshold = false;
	int FilterScaleX = 2;
	int FilterScaleY = 2;
	
	#include "includes/Init.h"

    double sinc(double x) {

        x = (x * M_PI);

        if (x < 0.01 && x > -0.01)
			return 1.0 + x * x *(-1.0 / 6.0 + x * x / 120.0);

        return std::sin(x) / x;
    }

    double LanczosFilter(double x, double lanczos_size) {

        if (std::abs(x) < lanczos_size) {

            return sinc(x) * sinc(x / lanczos_size);
        }

        return 0.0;
    }

    void Resize(unsigned char*& src, int srcw, int srch, double lanczos_size, unsigned char*& resized, int M, int N) {

        const int Channels = 3;

        auto col_ratio = (double)(srcw) / M;
        auto row_ratio = (double)(srch) / N;

        auto win = (int)(lanczos_size);

        // do not include alpha channel
        for (auto Channel = 0; Channel < Channels; Channel++) {

            // Now apply a filter to the image.
            for (auto r = 0; r < N; ++r) {

                auto row_within = (double)r * row_ratio;

                auto floor_row = (int)(row_within);

                for (auto c = 0; c < M; ++c) {

                    // x is the new col in terms of the old col coordinates.
                    auto col_within = (double)c * col_ratio;

                    // The old col corresponding to the closest new col.
                    auto floor_col = (int)(col_within);

                    auto v_toSet = 0.0;
                    auto weight = 0.0;

                    for (auto i = floor_row - win + 1; i <= floor_row + win; ++i) {
                        for (auto j = floor_col - win + 1; j <= floor_col + win; ++j) {

                            if (i >= 0 && i < srch && j >= 0 && j < srcw) {

                                auto lanc_term = LanczosFilter(row_within - i, lanczos_size) * LanczosFilter(col_within - j, lanczos_size);

                                v_toSet += (double)src[(i * srcw + j) * Channels + Channel] * lanc_term;

                                weight += lanc_term;
                            }
                        }
                    }

                    v_toSet /= weight;
					
					v_toSet = (v_toSet > 255.0) ? 255.0 : v_toSet;
					v_toSet = (v_toSet < 0.0) ? 0.0 : v_toSet;
			
                    resized[(r * M + c) * Channels + Channel] = (unsigned char)((int)(v_toSet));
                }
            }
        }
    }

	void Apply(int argc, void ** argv)
	{
		if (argc >= 4)
		{
			auto Input = ((unsigned char*)(argv[0]));
			auto srcx = *((int*)(argv[1]));
			auto srcy = *((int*)(argv[2]));
			auto scale = *((int*)(argv[3]));

			scale = std::max(1, scale);

			FilterScaleX = scale;
			FilterScaleY = scale;

			Init(srcx, srcy);
			
			auto win = 3.0;
			
			Resize(Input, srcx, srcy, win, ScaledImage, _SizeX, _SizeY);
		}
	}
}
