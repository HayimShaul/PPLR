CCFLAGS = -g --std=c++11 -Wall

NTLINCDIR = ../ntl-10.5.0-multithread/include
NTLLIBDIR = ../ntl-10.5.0-multithread/src

FHEINCDIR = ../HElib-multithread/src
FHELIBDIR = ../HElib-multithread/src

LIPHEINCDIR = ../liphe/include
LIPHELIBDIR = ../liphe/src

JSONDIR = -I/home/hayim/lib/json/src -I../json/src

LIBS = -L$(LIPHELIBDIR) -lliphe  $(FHELIBDIR)/fhe.a $(NTLLIBDIR)/ntl.a  -lgmp  -lpthread
INCS = $(JSONDIR) -I$(NTLINCDIR) -I$(FHEINCDIR) -I$(LIPHEINCDIR)

all: test_zp test_helib

test_zp: test_zp.o times.o
	g++ $(LDFLAGS) -o $@ $^ $(LIBS)

test_helib: test_helib.o times.o
	g++ $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cc
	g++ $(CCFLAGS) -c  $(INCS) $<

clean:
	rm -f *.o test_zp test_helib

