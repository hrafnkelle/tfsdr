CXXFLAGS = -g -Wall -ansi
IPP_PATH =  /opt/intel/ipp/6.0.1.071/ia32
INCLUDES = -I$(IPP_PATH)/include -I/home/he/private/libs/include
objects = AUFileAudioSink.o IQFileAudioSource.o 


all: $(objects)

$(objects): %.o: %.cpp
	$(CXX) -o $@ -c $(CXXFLAGS) $<  $(INCLUDES)

.PHONY: clean
clean:
	rm -f $(objects)
