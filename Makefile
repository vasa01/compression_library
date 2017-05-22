APP          = main
APP_SOURCES  = main.cpp

DEFINES         =

CFLAGS         =

LIBRARIES       = 

CC              = g++
APP_OBJECTS  = $(APP_SOURCES:.cpp=.o)
INCLUDES        = -I. -I/usr/local/include
LIBDIRS         = -lopencv_core -lopencv_imgproc -lopencv_highgui 
LDFLAGS         = $(LIBDIRS) $(LIBRARIES) 

###########################################

.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -c $<

###########################################

all:    $(APP)

rebuild:  clean all

$(APP):  $(APP_OBJECTS)
	$(CC) $(APP_OBJECTS)  $(LDFLAGS) -o $@


###########################################

clean:
	rm -f  $(APP)  *.o                   
