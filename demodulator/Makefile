CXXFLAGS = -g -Wall -ansi -static -pthread
IPP_PATH =  /opt/intel/ipp/6.0.1.071/ia32
INCLUDES = -I$(IPP_PATH)/include -I../IO -I~he/private/libs/include
LIBPATH = -L$(IPP_PATH)/lib/ -L. -L/home/he/private/libs/lib
IPPLIBS =  -lippsemerged -lippsmerged -lippacemerged -lippacmerged -lippcore
SNDLIBS = -lsndfile -lFLAC -ldemodulator -lvorbis -lvorbisenc -logg -lm
POSIXLIBS = -lrt
CGILIBS = -lcgicc

objects = IQSSBDemodulator.o sdr_test.o

all: $(objects) sdr_test libdemodulator.a

libdemodulator.a: IQSSBDemodulator.o
	ar cr libdemodulator.a IQSSBDemodulator.o

sdr_test: sdr_test.o libdemodulator.a
	$(CXX) $(CXXFLAGS) -o sdr_test $(objects) ../IO/*.o  $(INCLUDE) $(LIBPATH) $(IPPLIBS) $(SNDLIBS) $(POSIXLIBS) $(CGILIBS)



$(objects): %.o: %.cpp
	$(CXX) -o $@ -c $(CXXFLAGS) $<  $(INCLUDES)

.PHONY: clean
clean:
	rm -f $(objects)
