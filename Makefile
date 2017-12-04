

MAIN_SOURCE=dump_oas.cc 

SOURCES=\
  dbOASISDumper.cc \
  tlStream.cc \
  tlInternational.cc \
  tlVariant.cc \
  tlException.cc \
  tlString.cc \
  tlDeflate.cc \
  tlAssert.cc \
  dump_oas.cc \

CCDEFINES=
CCFLAGS=-O3 
CCSYSINCLUDE=-I/usr/include/qt4
LDFLAGS=-lstdc++ -lQtCore -L/usr/lib/qt4 -lz

%.o: %.cc
	gcc -o $@ -c $< $(CCDEFINES) $(CCFLAGS) $(CCSYSINCLUDE)

dump_oas: $(SOURCES:%.cc=%.o)
	g++ -o $@ $(SOURCES:%.cc=%.o) $(LDFLAGS)

clean:
	rm *.o dump_oas

depend:
	makedepend -- -Y $(CCDEFINES) -- $(SOURCES) 2>/dev/null

# DO NOT DELETE

dbOASISDumper.o: dbOASISDumper.h tlException.h config.h tlInternational.h
dbOASISDumper.o: tlVariant.h tlAssert.h tlStream.h tlString.h dbTypes.h
dbOASISDumper.o: dbPoint.h
tlStream.o: tlStream.h config.h tlException.h tlInternational.h tlVariant.h
tlStream.o: tlAssert.h tlString.h tlDeflate.h
tlInternational.o: tlInternational.h config.h tlString.h tlException.h
tlInternational.o: tlVariant.h tlAssert.h
tlVariant.o: tlVariant.h config.h tlInternational.h tlAssert.h tlString.h
tlVariant.o: tlException.h
tlException.o: tlException.h config.h tlInternational.h tlVariant.h
tlException.o: tlAssert.h tlString.h
tlString.o: tlString.h config.h tlException.h tlInternational.h tlVariant.h
tlString.o: tlAssert.h
tlDeflate.o: tlDeflate.h config.h tlStream.h tlException.h tlInternational.h
tlDeflate.o: tlVariant.h tlAssert.h tlString.h
tlAssert.o: tlAssert.h config.h tlException.h tlInternational.h tlVariant.h
dump_oas.o: dbOASISDumper.h tlException.h config.h tlInternational.h
dump_oas.o: tlVariant.h tlAssert.h tlStream.h tlString.h dbTypes.h dbPoint.h
