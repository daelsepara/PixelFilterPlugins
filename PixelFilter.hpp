#ifndef PIXELFILTER_HPP
#define PIXELFILTER_HPP

#include <cstdio>
#include <cstdlib>
#include <dlfcn.h>

class PixelFilter
{
public:

	unsigned char *ScaledImage = NULL;
	int ScaledX = 0;
	int ScaledY = 0;
	bool UseThreshold = false;
	
	void Init(int sizex, int sizey)
	{
		ScaledX = sizex;
		ScaledY = sizey;

		free(ScaledImage);

		ScaledImage = New(ScaledX, ScaledY);
	}
	
	void SetThreshold(bool threshold)
	{
		UseThreshold = threshold;
	} 

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
	
	void Free()
	{
		free(ScaledImage);
	}
	
	void Copy(unsigned char*& dst, unsigned char*& src, int Length)
	{
		memcpy(dst, src, Length * sizeof(unsigned char));
	}
	
	void Filter(const char* filter, int argc, void** argv)
	{
		auto Channels = 3;
		
		char Libname[200];
			
		void *lib_handle;

		int (*SizeX)(void);
		int (*SizeY)(void);
		void (*Threshold)(bool);
		void (*Apply)(int, void**);
		unsigned char* (*Image)(void);
		void (*Release)(void);
		
		char *error;

		#if OSX
			sprintf(Libname, "./libpixel++%s.dylib", filter);
		#else
			sprintf(Libname, "./libpixel++%s.so", filter);
		#endif

		lib_handle = dlopen(Libname, RTLD_LAZY);

		if (!lib_handle)
		{
		  fprintf(stderr, "%s\n", dlerror());
		  exit(1);
		}
		
		SizeX = (int(*)(void)) dlsym(lib_handle, "SizeX");
		if ((error = dlerror()) != NULL) 
		{
		  fprintf(stderr, "%s\n", error);
		  exit(1);
		}
		
		SizeY = (int(*)(void)) dlsym(lib_handle, "SizeY");
		if ((error = dlerror()) != NULL) 
		{
		  fprintf(stderr, "%s\n", error);
		  exit(1);
		}
		
		Threshold = (void(*)(bool)) dlsym(lib_handle, "Threshold");
		if ((error = dlerror()) != NULL) 
		{
		  fprintf(stderr, "%s\n", error);
		  exit(1);
		}
		
		Apply = (void(*)(int, void**)) dlsym(lib_handle, "Apply");
		if ((error = dlerror()) != NULL) 
		{
		  fprintf(stderr, "%s\n", error);
		  exit(1);
		}
		
		Image = (unsigned char*(*)(void)) dlsym(lib_handle, "Image");
		if ((error = dlerror()) != NULL) 
		{
		  fprintf(stderr, "%s\n", error);
		  exit(1);
		}
		
		Release = (void(*)(void)) dlsym(lib_handle, "Release");
		if ((error = dlerror()) != NULL) 
		{
		  fprintf(stderr, "%s\n", error);
		  exit(1);
		}
		
		(*Threshold)(UseThreshold);
		
		(*Apply)(argc, argv);

		auto sizex = (*SizeX)();
		auto sizey = (*SizeY)();

		auto scaled = (*Image)();

		auto length = sizex * sizey * Channels;

		Init(sizex, sizey);
						
		Copy(ScaledImage, scaled, length);
		
		(*Release)();
		
		dlclose(lib_handle);
	}
};
#endif
