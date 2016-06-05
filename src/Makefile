CC = g++ -std=c++11
OBJECTS = main.o Image.o ContourDetector.o DistanceTransformer.o FeatureExtraction.o ImageFilter.o ImageFramework.o ImageLoader.o ImageManipulation.o ImageSaver.o ImageSearch.o ImageStatistics.o IntegralImage.o PartRecognizer.o PartTemplate.o
LIBS = -lm -lpng12 -ljpeg
CFLAGS = -pedantic -O2 -I../include
BINDIR = $(DESTDIR)/usr/bin
NAME = main

main: $(OBJECTS)
	$(CC) -o $(NAME) $(OBJECTS) $(LIBS)

%.o: %.cpp
	$(CC) -c $(CFLAGS) $<

