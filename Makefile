

SOURCES=\
  dbOASISDumper.cc \
  dbGDS2Dumper.cc \
  tlStream.cc \
  tlVariant.cc \
  tlException.cc \
  tlString.cc \
  tlDeflate.cc \
  tlAssert.cc \

CCDEFINES=
CCFLAGS=-O3 
LDFLAGS=-lstdc++ -lz

all: dump_oas dump_gds2 

%.o: %.cc
	gcc -o $@ -c $< $(CCDEFINES) $(CCFLAGS)

dump_oas: dump_oas.o $(SOURCES:%.cc=%.o)
	g++ -o $@ $^ $(LDFLAGS)

dump_gds2: dump_gds2.o $(SOURCES:%.cc=%.o)
	g++ -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o dump_oas dump_gds2

depend:
	makedepend -- -Y $(CCDEFINES) -- $(SOURCES) 2>/dev/null

# DO NOT DELETE

dbOASISDumper.o: dbOASISDumper.h tlException.h config.h tlVariant.h
dbOASISDumper.o: tlAssert.h tlStream.h tlString.h dbTypes.h dbPoint.h
dbGDS2Dumper.o: dbGDS2Dumper.h tlException.h config.h tlVariant.h tlAssert.h
dbGDS2Dumper.o: tlStream.h tlString.h dbTypes.h dbPoint.h
tlStream.o: tlStream.h config.h tlException.h tlVariant.h tlAssert.h
tlStream.o: tlString.h tlDeflate.h
tlVariant.o: tlVariant.h config.h tlAssert.h tlString.h tlException.h
tlException.o: tlException.h config.h tlVariant.h tlAssert.h tlString.h
tlString.o: tlString.h config.h tlException.h tlVariant.h tlAssert.h
tlDeflate.o: tlDeflate.h config.h tlStream.h tlException.h tlVariant.h
tlDeflate.o: tlAssert.h tlString.h
tlAssert.o: tlAssert.h config.h tlException.h tlVariant.h
