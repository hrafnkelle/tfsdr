CXXFLAGS = -g -O0 -ansi -static -pthread
#CXXFLAGS = -O3 -march=i686 -msse2 -Wall -ansi -static -pthread
IPP_PATH = /opt/intel/ipp/6.0.1.071/ia32
INCLUDE = -I$(IPP_PATH)/include -I../IO -I../demodulator -I/home/he/private/libs/include
LIBPATH = -L$(IPP_PATH)/lib/ -L../demodulator -L/home/he/private/libs/lib/
IPPLIBS =  -lippsemerged -lippsmerged -lippacemerged -lippacmerged -lippcore
SNDLIBS = -lsndfile -lFLAC -ldemodulator -lvorbis -lvorbisenc -logg -lm
POSIXLIBS = -lrt
CGILIBS = -lcgicc

objects = sdrfeed.o sdrtune.o md5.o md5wrapper.o sdrwaterfall.o session.o sdrsession.o


all: sdrfeed sdrtune sdrwaterfall sdrsession

sdrfeed: sdrfeed.o md5.o md5wrapper.o session.o
	$(CXX) $(CXXFLAGS) -o sdrfeed $^ ../IO/*.o $(INCLUDE) $(LIBPATH) $(SNDLIBS) $(IPPLIBS) $(POSIXLIBS) $(CGILIBS)

sdrsession: sdrsession.o session.o md5.o md5wrapper.o
	$(CXX) $(CXXFLAGS) -o sdrsession $^ $(POSIXLIBS) $(CGILIBS)

sdrtune: sdrtune.o session.o md5.o md5wrapper.o
	$(CXX) $(CXXFLAGS) -o sdrtune $^ $(POSIXLIBS) $(CGILIBS)

sdrwaterfall: sdrwaterfall.o
	$(CXX) $(CXXFLAGS) -o sdrwaterfall $< ../IO/*.o $(LIBPATH) $(IPPLIBS) $(SNDLIBS) $(POSIXLIBS) $(CGILIBS)

$(objects): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<  $(INCLUDE)

install: sdrtune sdrfeed sdrsession sdrwaterfall
	cp sdrfeed ~/public_html/cgi-bin/
	cp sdrtune ~/public_html/cgi-bin/
	cp sdrwaterfall ~/public_html/cgi-bin/
	cp sdrsession ~/public_html/cgi-bin/

.PHONY: clean
clean:
	$(RM) *.o sdrtune sdrfeed

