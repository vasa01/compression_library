APP          = main
APP_SOURCES  = main.cpp compressionLibrary.cpp compressionLibrary.hpp cdf97.cpp cdf97.hpp cdf53.cpp cdf53.hpp ari.hpp ariDecode.cpp ariDecode.hpp ariEncode.cpp ariEncode.hpp bitIOStream.hpp EZWDecode.cpp EZWDecode.hpp EZWEncode.cpp EZWEncode.hpp bitStream.hpp bivector.hpp block_coder.cpp block_coder.hpp block_decoder.hpp block_encoder.hpp contourlet.cpp contourlet.hpp cplx.cpp cplx.hpp dfb.cpp dfb.hpp distance.cpp distance.hpp dwtImage.hpp dwtLevels.hpp fileIOStream.hpp io.cpp io.hpp iOStream.hpp joinSubimage.hpp line.hpp load_mat.cpp load_mat.hpp mat.cpp mat.hpp math.cpp math.hpp matrix.hpp mq_coder.cpp mq_coder.hpp mq_decoder2.cpp mq_decoder2.hpp mq_encoder2.cpp mq_encoder2.hpp mycontourlet.cpp mycountourlet.hpp poly.cpp poly.hpp predicter.hpp psnr.cpp psnr.hpp pyramid.cpp pyramid.hpp random.cpp random.hpp save_mat.cpp save_mat.hpp source_func.cpp source_func.hpp spiht.hpp spihtDecode.cpp spihtDecode.hpp spihtEncode.cpp spihtEncode.hpp ssim.cpp ssim.hpp tool.hpp types.hpp vec.cpp vec.hpp

DEFINES         =

CFLAGS         = -std=c++11
LIBRARIES       = 

CC              = g++
APP_OBJECTS  = $(APP_SOURCES:.cpp=.o)
INCLUDES        = -I. -I/usr/local/include
LIBDIRS         = -lopencv_core -lopencv_imgproc -lopencv_highgui 
LDFLAGS         = $(LIBDIRS) $(LIBRARIES) 

###########################################

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) -w -c $<

###########################################

all:    $(APP)

rebuild:  clean all

$(APP):  $(APP_OBJECTS)
	$(CC) $(CFLAGS)  $(APP_OBJECTS)  $(LDFLAGS) -o $@


###########################################
run:
	./main -i ./images/lena.bmp -o "decode.bmp" -t 0 -e 2 -s 20 -q 1 -l 4



###########################################

clean:
	rm -f  $(APP)  *.o                   
