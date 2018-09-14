export UNAME = $(shell uname)

MAJ_VERSION=1
MIN_VERSION=0
FUL_VERSION=$(MAJ_VERSION).$(MIN_VERSION)

ifeq ($(UNAME), Darwin)

LFLAGS_LIB=-dynamic \
	-dynamiclib \
	-framework Carbon \
	-framework CoreFoundation \
	-framework IOKit \
	-o

CFLAGS_BASE=-c \
	-Wall \
	-Wunused \
	-Wmissing-include-dirs \
	-Werror \
	-g \
	-O0 \
	-fpic \
	-fno-stack-protector \
	-std=c++11 \
	-Wc++11-extensions
	
EXEC_FLAGS=-std=c++11 -ldl -DOSX -Wc++11-extensions -o
	
else

CFLAGS_BASE=-Wall -fPIC -std=c++11 -Wc++11-extensions -c

LFLAGS_LIB=-shared -Wl,-soname

EXEC_FLAGS=-std=c++11 -rdynamic -ldl -Wc++11-extensions -o

endif

OPENCV_INCLUDES=-I/usr/local/include -I/usr/include -I/usr/include/opencv2 -I/usr/local/include/opencv-3.4.2/
OPENCV_LIBS=-L/usr/local/lib -L/usr/lib -lopencv_core -lopencv_highgui -lopencv_imgcodecs

all: test PixelFilter \
	epx kuwahara xbrz hqx lqx xbr \
	sai supersai eagle supereagle \
	rgb tv advmameinterp scale3x \
	magnify amscale horiz vertscan \
	reverseaa tvzero \
	ultra2x super2x 2xscl des2x des \
	bilinear bilinearplus bilinearpp \
	hq2xn lq2xn epxb epxc eagle3xb \
	flip rotate nearest
	
PixelFilter:
	clang++ $(EXEC_FLAGS) PixelFilter.exe PixelFilter.cpp lodepng.cpp $(OPENCV_INCLUDES) $(OPENCV_LIBS)
	
test:
	clang++ $(EXEC_FLAGS) test.exe test.cpp
	
epx:
	clang++ $(CFLAGS_BASE) epx.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++epx.$(FUL_VERSION).dylib epx.o
	ln -sf libpixel++epx.$(FUL_VERSION).dylib libpixel++epx.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++epx.so.$(MAJ_VERSION) -o libpixel++epx.so.$(FUL_VERSION) epx.o
	ln -sf libpixel++epx.so.$(FUL_VERSION) libpixel++epx.so
endif

kuwahara:
	clang++ $(CFLAGS_BASE) kuwahara.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++kuwahara.$(FUL_VERSION).dylib kuwahara.o
	ln -sf libpixel++kuwahara.$(FUL_VERSION).dylib libpixel++kuwahara.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++kuwahara.so.$(MAJ_VERSION) -o libpixel++kuwahara.so.$(FUL_VERSION) kuwahara.o
	ln -sf libpixel++kuwahara.so.$(FUL_VERSION) libpixel++kuwahara.so
endif

xbrz:
	clang++ $(CFLAGS_BASE) xbrz.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++xbrz.$(FUL_VERSION).dylib xbrz.o
	ln -sf libpixel++xbrz.$(FUL_VERSION).dylib libpixel++xbrz.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++xbrz.so.$(MAJ_VERSION) -o libpixel++xbrz.so.$(FUL_VERSION) xbrz.o
	ln -sf libpixel++xbrz.so.$(FUL_VERSION) libpixel++xbrz.so
endif

hqx:
	clang++ $(CFLAGS_BASE) hqnx.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++hqx.$(FUL_VERSION).dylib hqnx.o
	ln -sf libpixel++hqx.$(FUL_VERSION).dylib libpixel++hqx.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++hqx.so.$(MAJ_VERSION) -o libpixel++hqx.so.$(FUL_VERSION) hqnx.o
	ln -sf libpixel++hqx.so.$(FUL_VERSION) libpixel++hqx.so
endif

lqx:
	clang++ $(CFLAGS_BASE) lqnx.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++lqx.$(FUL_VERSION).dylib lqnx.o
	ln -sf libpixel++lqx.$(FUL_VERSION).dylib libpixel++lqx.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++lqx.so.$(MAJ_VERSION) -o libpixel++lqx.so.$(FUL_VERSION) lqnx.o
	ln -sf libpixel++lqx.so.$(FUL_VERSION) libpixel++lqx.so
endif

xbr:
	clang++ $(CFLAGS_BASE) xbr.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++xbr.$(FUL_VERSION).dylib xbr.o
	ln -sf libpixel++xbr.$(FUL_VERSION).dylib libpixel++xbr.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++xbr.so.$(MAJ_VERSION) -o libpixel++xbr.so.$(FUL_VERSION) xbr.o
	ln -sf libpixel++xbr.so.$(FUL_VERSION) libpixel++xbr.so
endif

sai:
	clang++ $(CFLAGS_BASE) 2xsai.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++sai.$(FUL_VERSION).dylib 2xsai.o
	ln -sf libpixel++sai.$(FUL_VERSION).dylib libpixel++sai.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++sai.so.$(MAJ_VERSION) -o libpixel++sai.so.$(FUL_VERSION) 2xsai.o
	ln -sf libpixel++sai.so.$(FUL_VERSION) libpixel++sai.so
endif

supersai:
	clang++ $(CFLAGS_BASE) supersai.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++supersai.$(FUL_VERSION).dylib supersai.o
	ln -sf libpixel++supersai.$(FUL_VERSION).dylib libpixel++supersai.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++supersai.so.$(MAJ_VERSION) -o libpixel++supersai.so.$(FUL_VERSION) supersai.o
	ln -sf libpixel++supersai.so.$(FUL_VERSION) libpixel++supersai.so
endif

eagle:
	clang++ $(CFLAGS_BASE) eagle.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++eagle.$(FUL_VERSION).dylib eagle.o
	ln -sf libpixel++eagle.$(FUL_VERSION).dylib libpixel++eagle.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++eagle.so.$(MAJ_VERSION) -o libpixel++eagle.so.$(FUL_VERSION) eagle.o
	ln -sf libpixel++eagle.so.$(FUL_VERSION) libpixel++eagle.so
endif

supereagle:
	clang++ $(CFLAGS_BASE) supereagle.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++supereagle.$(FUL_VERSION).dylib supereagle.o
	ln -sf libpixel++supereagle.$(FUL_VERSION).dylib libpixel++supereagle.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++supereagle.so.$(MAJ_VERSION) -o libpixel++supereagle.so.$(FUL_VERSION) supereagle.o
	ln -sf libpixel++supereagle.so.$(FUL_VERSION) libpixel++supereagle.so
endif

rgb:
	clang++ $(CFLAGS_BASE) rgb.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++rgb.$(FUL_VERSION).dylib rgb.o
	ln -sf libpixel++rgb.$(FUL_VERSION).dylib libpixel++rgb.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++rgb.so.$(MAJ_VERSION) -o libpixel++rgb.so.$(FUL_VERSION) rgb.o
	ln -sf libpixel++rgb.so.$(FUL_VERSION) libpixel++rgb.so
endif

tv:
	clang++ $(CFLAGS_BASE) tv.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++tv.$(FUL_VERSION).dylib tv.o
	ln -sf libpixel++tv.$(FUL_VERSION).dylib libpixel++tv.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++tv.so.$(MAJ_VERSION) -o libpixel++tv.so.$(FUL_VERSION) tv.o
	ln -sf libpixel++tv.so.$(FUL_VERSION) libpixel++tv.so
endif

advmameinterp:
	clang++ $(CFLAGS_BASE) advmameinterp.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++advmameinterp.$(FUL_VERSION).dylib advmameinterp.o
	ln -sf libpixel++advmameinterp.$(FUL_VERSION).dylib libpixel++advmameinterp.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++advmameinterp.so.$(MAJ_VERSION) -o libpixel++advmameinterp.so.$(FUL_VERSION) advmameinterp.o
	ln -sf libpixel++advmameinterp.so.$(FUL_VERSION) libpixel++advmameinterp.so
endif

scale3x:
	clang++ $(CFLAGS_BASE) scale3x.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++scale3x.$(FUL_VERSION).dylib scale3x.o
	ln -sf libpixel++scale3x.$(FUL_VERSION).dylib libpixel++scale3x.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++scale3x.so.$(MAJ_VERSION) -o libpixel++scale3x.so.$(FUL_VERSION) scale3x.o
	ln -sf libpixel++scale3x.so.$(FUL_VERSION) libpixel++scale3x.so
endif

magnify:
	clang++ $(CFLAGS_BASE) magnify.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++magnify.$(FUL_VERSION).dylib magnify.o
	ln -sf libpixel++magnify.$(FUL_VERSION).dylib libpixel++magnify.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++magnify.so.$(MAJ_VERSION) -o libpixel++magnify.so.$(FUL_VERSION) magnify.o
	ln -sf libpixel++magnify.so.$(FUL_VERSION) libpixel++magnify.so
endif

amscale:
	clang++ $(CFLAGS_BASE) amscale.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++amscale.$(FUL_VERSION).dylib amscale.o
	ln -sf libpixel++amscale.$(FUL_VERSION).dylib libpixel++amscale.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++amscale.so.$(MAJ_VERSION) -o libpixel++amscale.so.$(FUL_VERSION) amscale.o
	ln -sf libpixel++amscale.so.$(FUL_VERSION) libpixel++amscale.so
endif

horiz:
	clang++ $(CFLAGS_BASE) horiz.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++horiz.$(FUL_VERSION).dylib horiz.o
	ln -sf libpixel++horiz.$(FUL_VERSION).dylib libpixel++horiz.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++horiz.so.$(MAJ_VERSION) -o libpixel++horiz.so.$(FUL_VERSION) horiz.o
	ln -sf libpixel++horiz.so.$(FUL_VERSION) libpixel++horiz.so
endif

vertscan:
	clang++ $(CFLAGS_BASE) vertscan.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++vertscan.$(FUL_VERSION).dylib vertscan.o
	ln -sf libpixel++vertscan.$(FUL_VERSION).dylib libpixel++vertscan.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++vertscan.so.$(MAJ_VERSION) -o libpixel++vertscan.so.$(FUL_VERSION) vertscan.o
	ln -sf libpixel++vertscan.so.$(FUL_VERSION) libpixel++vertscan.so
endif

reverseaa:
	clang++ $(CFLAGS_BASE) reverseaa.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++reverseaa.$(FUL_VERSION).dylib reverseaa.o
	ln -sf libpixel++reverseaa.$(FUL_VERSION).dylib libpixel++reverseaa.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++reverseaa.so.$(MAJ_VERSION) -o libpixel++reverseaa.so.$(FUL_VERSION) reverseaa.o
	ln -sf libpixel++reverseaa.so.$(FUL_VERSION) libpixel++reverseaa.so
endif

ultra2x:
	clang++ $(CFLAGS_BASE) ultra2x.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++ultra2x.$(FUL_VERSION).dylib ultra2x.o
	ln -sf libpixel++ultra2x.$(FUL_VERSION).dylib libpixel++ultra2x.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++ultra2x.so.$(MAJ_VERSION) -o libpixel++ultra2x.so.$(FUL_VERSION) ultra2x.o
	ln -sf libpixel++ultra2x.so.$(FUL_VERSION) libpixel++ultra2x.so
endif

super2x:
	clang++ $(CFLAGS_BASE) super2x.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++super2x.$(FUL_VERSION).dylib super2x.o
	ln -sf libpixel++super2x.$(FUL_VERSION).dylib libpixel++super2x.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++super2x.so.$(MAJ_VERSION) -o libpixel++super2x.so.$(FUL_VERSION) super2x.o
	ln -sf libpixel++super2x.so.$(FUL_VERSION) libpixel++super2x.so
endif

2xscl:
	clang++ $(CFLAGS_BASE) 2xscl.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++2xscl.$(FUL_VERSION).dylib 2xscl.o
	ln -sf libpixel++2xscl.$(FUL_VERSION).dylib libpixel++2xscl.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++2xscl.so.$(MAJ_VERSION) -o libpixel++2xscl.so.$(FUL_VERSION) 2xscl.o
	ln -sf libpixel++2xscl.so.$(FUL_VERSION) libpixel++2xscl.so
endif

des2x:
	clang++ $(CFLAGS_BASE) des2x.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++des2x.$(FUL_VERSION).dylib des2x.o
	ln -sf libpixel++des2x.$(FUL_VERSION).dylib libpixel++des2x.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++des2x.so.$(MAJ_VERSION) -o libpixel++des2x.so.$(FUL_VERSION) des2x.o
	ln -sf libpixel++des2x.so.$(FUL_VERSION) libpixel++des2x.so
endif

des:
	clang++ $(CFLAGS_BASE) des.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++des.$(FUL_VERSION).dylib des.o
	ln -sf libpixel++des.$(FUL_VERSION).dylib libpixel++des.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++des.so.$(MAJ_VERSION) -o libpixel++des.so.$(FUL_VERSION) des.o
	ln -sf libpixel++des.so.$(FUL_VERSION) libpixel++des.so
endif

bilinear:
	clang++ $(CFLAGS_BASE) bilinear.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++bilinear.$(FUL_VERSION).dylib bilinear.o
	ln -sf libpixel++bilinear.$(FUL_VERSION).dylib libpixel++bilinear.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++bilinear.so.$(MAJ_VERSION) -o libpixel++bilinear.so.$(FUL_VERSION) bilinear.o
	ln -sf libpixel++bilinear.so.$(FUL_VERSION) libpixel++bilinear.so
endif

bilinearplus:
	clang++ $(CFLAGS_BASE) bilinearplus.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++bilinearplus.$(FUL_VERSION).dylib bilinearplus.o
	ln -sf libpixel++bilinearplus.$(FUL_VERSION).dylib libpixel++bilinearplus.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++bilinearplus.so.$(MAJ_VERSION) -o libpixel++bilinearplus.so.$(FUL_VERSION) bilinearplus.o
	ln -sf libpixel++bilinearplus.so.$(FUL_VERSION) libpixel++bilinearplus.so
endif

bilinearpp:
	clang++ $(CFLAGS_BASE) bilinearpp.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++bilinearpp.$(FUL_VERSION).dylib bilinearpp.o
	ln -sf libpixel++bilinearpp.$(FUL_VERSION).dylib libpixel++bilinearpp.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++bilinearpp.so.$(MAJ_VERSION) -o libpixel++bilinearpp.so.$(FUL_VERSION) bilinearpp.o
	ln -sf libpixel++bilinearpp.so.$(FUL_VERSION) libpixel++bilinearpp.so
endif

hq2xn:
	clang++ $(CFLAGS_BASE) hq2xn.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++hq2xn.$(FUL_VERSION).dylib hq2xn.o
	ln -sf libpixel++hq2xn.$(FUL_VERSION).dylib libpixel++hq2xn.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++hq2xn.so.$(MAJ_VERSION) -o libpixel++hq2xn.so.$(FUL_VERSION) hq2xn.o
	ln -sf libpixel++hq2xn.so.$(FUL_VERSION) libpixel++hq2xn.so
endif

lq2xn:
	clang++ $(CFLAGS_BASE) lq2xn.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++lq2xn.$(FUL_VERSION).dylib lq2xn.o
	ln -sf libpixel++lq2xn.$(FUL_VERSION).dylib libpixel++lq2xn.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++lq2xn.so.$(MAJ_VERSION) -o libpixel++lq2xn.so.$(FUL_VERSION) lq2xn.o
	ln -sf libpixel++lq2xn.so.$(FUL_VERSION) libpixel++lq2xn.so
endif

epxb:
	clang++ $(CFLAGS_BASE) epxb.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++epxb.$(FUL_VERSION).dylib epxb.o
	ln -sf libpixel++epxb.$(FUL_VERSION).dylib libpixel++epxb.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++epxb.so.$(MAJ_VERSION) -o libpixel++epxb.so.$(FUL_VERSION) epxb.o
	ln -sf libpixel++epxb.so.$(FUL_VERSION) libpixel++epxb.so
endif

epxc:
	clang++ $(CFLAGS_BASE) epxc.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++epxc.$(FUL_VERSION).dylib epxc.o
	ln -sf libpixel++epxc.$(FUL_VERSION).dylib libpixel++epxc.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++epxc.so.$(MAJ_VERSION) -o libpixel++epxc.so.$(FUL_VERSION) epxc.o
	ln -sf libpixel++epxc.so.$(FUL_VERSION) libpixel++epxc.so
endif

eagle3xb:
	clang++ $(CFLAGS_BASE) eagle3xb.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++eagle3xb.$(FUL_VERSION).dylib eagle3xb.o
	ln -sf libpixel++eagle3xb.$(FUL_VERSION).dylib libpixel++eagle3xb.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++eagle3xb.so.$(MAJ_VERSION) -o libpixel++eagle3xb.so.$(FUL_VERSION) eagle3xb.o
	ln -sf libpixel++eagle3xb.so.$(FUL_VERSION) libpixel++eagle3xb.so
endif

flip:
	clang++ $(CFLAGS_BASE) flip.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++flip.$(FUL_VERSION).dylib flip.o
	ln -sf libpixel++flip.$(FUL_VERSION).dylib libpixel++flip.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++flip.so.$(MAJ_VERSION) -o libpixel++flip.so.$(FUL_VERSION) flip.o
	ln -sf libpixel++flip.so.$(FUL_VERSION) libpixel++flip.so
endif

rotate:
	clang++ $(CFLAGS_BASE) rotate.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++rotate.$(FUL_VERSION).dylib rotate.o
	ln -sf libpixel++rotate.$(FUL_VERSION).dylib libpixel++rotate.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++rotate.so.$(MAJ_VERSION) -o libpixel++rotate.so.$(FUL_VERSION) rotate.o
	ln -sf libpixel++rotate.so.$(FUL_VERSION) libpixel++rotate.so
endif

nearest:
	clang++ $(CFLAGS_BASE) nearest.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++nearest.$(FUL_VERSION).dylib nearest.o
	ln -sf libpixel++nearest.$(FUL_VERSION).dylib libpixel++nearest.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++nearest.so.$(MAJ_VERSION) -o libpixel++nearest.so.$(FUL_VERSION) nearest.o
	ln -sf libpixel++nearest.so.$(FUL_VERSION) libpixel++nearest.so
endif

tvzero:
	clang++ $(CFLAGS_BASE) tvzero.cpp
	
ifeq ($(UNAME), Darwin)
	clang++ $(LFLAGS_LIB) libpixel++tvzero.$(FUL_VERSION).dylib tvzero.o
	ln -sf libpixel++tvzero.$(FUL_VERSION).dylib libpixel++tvzero.dylib
else
	clang++ $(LFLAGS_LIB),libpixel++tvzero.so.$(MAJ_VERSION) -o libpixel++tvzero.so.$(FUL_VERSION) tvzero.o
	ln -sf libpixel++tvzero.so.$(FUL_VERSION) libpixel++tvzero.so
endif

cleanup:
	rm -f *.o
	
install: all cleanup
	
clean:
	rm -f *.o *.exe *.so *.so.* *.dylib
