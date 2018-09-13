	#pragma once
	
	unsigned char* Buffer(int Length, unsigned char c)
	{
		auto Channels = 3;

		auto buffer = (unsigned char*)malloc(Length * Channels);

		for (auto i = 0; i < Length; i++)
		{
			auto index = i * Channels;

			buffer[index] = c;
			buffer[index + 1] = c;
			buffer[index + 2] = c;
		}

		return buffer;
	}

	unsigned char* New(int x, int y)
	{
		return Buffer(x * y, (unsigned char)0);
	}

	void Init(int srcx, int srcy)
	{
		_ScaleX = FilterScaleX;
		_ScaleY = FilterScaleY;
		_SizeX = srcx * FilterScaleX;
		_SizeY = srcy * FilterScaleY;
		_Threshold = ComparisonThreshold;

		free(ScaledImage);

		ScaledImage = New(_SizeX, _SizeY);
	}

	extern "C" int ID()
	{
		return FilterID;
	}


	extern "C" int ScaleX()
	{
		return FilterScaleX;
	}

	extern "C" int ScaleY()
	{
		return FilterScaleY;
	}

	extern "C" int SizeX()
	{
		return _SizeX;
	}

	extern "C" int SizeY()
	{
		return _SizeY;
	}

	extern "C" const char* Name()
	{
		return FilterName;
	}

	extern "C" const char* Description()
	{
		return FilterDescription;
	}

	extern "C" unsigned char* Image()
	{
		return ScaledImage;
	}

	extern "C" void Threshold(bool threshold)
	{
		ComparisonThreshold = threshold;
	}
	
	extern "C" void Release()
	{
		free(ScaledImage);

		ScaledImage = NULL;
	}
