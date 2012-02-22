CXX =g++
CTAGS=ctags --c++-kinds=+pl --fields=+iaS --extra=+q .

CXXFLAGS =-Wall -g

INCLUDES = streamrecorder.h rtp.h h264.h recorder.h httpserver.h \
		   thread.h webfrontend.h xmltv.h pugixml/pugixml.hpp settings.h

SOURCES = streamrecorder.cpp rtp.cpp h264.cpp recorder.cpp httpserver.cpp \
		  thread.cpp webfrontend.cpp xmltv.cpp pugixml/pugixml.cpp settings.cpp

OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = streamrecorder 

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(INCLUDES) $(OBJECTS) 
	$(CXX) -o $@ $(OBJECTS) -lpthread 

clean:
	rm *.o pugixml/*.o tags $(EXECUTABLE); $(CTAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<

