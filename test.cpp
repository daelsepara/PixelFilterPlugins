#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main(int argc, char **argv)
{
	if (argc > 1)
	{
		char Libname[200];
		
		void *lib_handle;

		int (*ID)(void);
		int (*ScaleX)(void);
		int (*ScaleY)(void);
		const char* (*Name)(void);
		const char* (*Description)(void);

		char *error;

		#if OSX
			sprintf(Libname, "./libpixel++%s.dylib", argv[1]);
		#else
			sprintf(Libname, "./libpixel++%s.so", argv[1]);
		#endif

		lib_handle = dlopen(Libname, RTLD_LAZY);

		if (!lib_handle)
		{
		  fprintf(stderr, "%s\n", dlerror());
		  exit(1);
		}

		ID = (int(*)(void)) dlsym(lib_handle, "ID");
		if ((error = dlerror()) != NULL) 
		{
		  fprintf(stderr, "%s\n", error);
		  exit(1);
		}

		printf("Filter ID = %x\n",(*ID)());
		
		Name = (const char*(*)(void)) dlsym(lib_handle, "Name");
		if ((error = dlerror()) != NULL) 
		{
		  fprintf(stderr, "%s\n", error);
		  exit(1);
		}

		printf("Name = %s\n",(*Name)());

		Description = (const char*(*)(void)) dlsym(lib_handle, "Description");
		if ((error = dlerror()) != NULL) 
		{
		  fprintf(stderr, "%s\n", error);
		  exit(1);
		}

		printf("Description = %s\n",(*Description)());

		ScaleX = (int(*)(void)) dlsym(lib_handle, "ScaleX");
		if ((error = dlerror()) != NULL) 
		{
		  fprintf(stderr, "%s\n", error);
		  exit(1);
		}

		printf("ScaleX = %d\n",(*ScaleX)());

		ScaleY = (int(*)(void)) dlsym(lib_handle, "ScaleY");
		if ((error = dlerror()) != NULL) 
		{
		  fprintf(stderr, "%s\n", error);
		  exit(1);
		}

		printf("ScaleY = %d\n",(*ScaleY)());

		dlclose(lib_handle);

		return 0;
	}

	return -1;
}
