CPPFLAGS=-g $(shell root-config --cflags) -std=c++11
LDFLAGS=-g $(shell root-config --ldflags)

########LIBRAIRIES
LIBS_ffmpeg = -lm -lz -lpthread -lavformat -lavcodec -lavutil
LIBS_opencv = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs
LIBS_autres = -lpthread -ldl -lm
LIBS = -L/usr/local/lib/ $(LIBS_autres) $(LIBS_ffmpeg) $(LIBS_opencv)

test: main.o ImageFeature.o
	g++ $(LDFLAGS) -o main main.o ImageFeature.o $(LIBS) 

%.o: %.cpp
	g++ $(CPPFLAGS) -I/usr/local/include -c $<
