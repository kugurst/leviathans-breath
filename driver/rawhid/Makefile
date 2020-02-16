
OS = LINUX
#OS = MACOSX
#OS = WINDOWS

PROG = python_binding
# PROG = test

# To set up Ubuntu Linux to cross compile for Windows:
#
#    apt-get install mingw32 mingw32-binutils mingw32-runtime
#
# Just edit the variable above for WINDOWS, then use "make" to build rawhid.exe

ifeq ($(OS), LINUX)
TARGET = $(PROG).so
CXX = clang++
CC = clang
# STRIP = strip
CFLAGS = -Wall -O2 -DOS_$(OS) -I./FixedPoints/src -I/usr/include/python3.7m -DDRIVER -DFIXED_POINTS_USE_NAMESPACE -g -DDISABLE_SERIAL -fPIC
CXXFLAGS = -Wall -O2 -DOS_$(OS) -I./FixedPoints/src -I/usr/include/python3.7m -DDRIVER -DFIXED_POINTS_USE_NAMESPACE -g -DDISABLE_SERIAL -fPIC
LDFLAGS = -g
LIBS = -lusb -lgtest -lstdc++ -lpython3.7m -lboost_python37
else ifeq ($(OS), MACOSX)
TARGET = $(PROG).dmg
SDK = /Developer/SDKs/MacOSX10.5.sdk
ARCH = -mmacosx-version-min=10.5 -arch ppc -arch i386
CXX = clang++
CC = clang
# STRIP = strip
CFLAGS = -Wall -O2 -DOS_$(OS) -isysroot $(SDK) $(ARCH) -I./FixedPoints/src -DDRIVER -DFIXED_POINTS_USE_NAMESPACE -g -DDISABLE_SERIAL
CXXFLAGS = -Wall -O2 -DOS_$(OS) -isysroot $(SDK) $(ARCH) -I./FixedPoints/src -DDRIVER -DFIXED_POINTS_USE_NAMESPACE -g -DDISABLE_SERIAL
LDFLAGS = -g
LIBS = $(ARCH) -Wl,-syslibroot,$(SDK) -framework IOKit -framework CoreFoundation -lgtest
else ifeq ($(OS), WINDOWS)
TARGET = $(PROG).exe
CC = i586-mingw32msvc-gcc
# STRIP = 	i586-mingw32msvc-strip
CFLAGS = -Wall -O2 -DOS_$(OS) -I./FixedPoints/src -DDRIVER -DFIXED_POINTS_USE_NAMESPACE -g -DDISABLE_SERIAL
CXXFLAGS = -Wall -O2 -DOS_$(OS) -I./FixedPoints/src -DDRIVER -DFIXED_POINTS_USE_NAMESPACE -g -DDISABLE_SERIAL
LDFLAGS = -g
LIBS = -lhid -lsetupapi -lgtest
endif

OBJS = $(PROG).o hid.o driver.o


all: $(TARGET)

$(PROG): $(OBJS)
	$(CC) -g -Wall -o $(PROG) $(OBJS) $(LIBS)
	# $(STRIP) $(PROG)

$(PROG).exe: $(PROG)
	cp $(PROG) $(PROG).exe

$(PROG).so: $(PROG).o
	$(CC) -g -Wall -shared -o $(PROG).so $(OBJS) $(LIBS)
	cp $(PROG).so leviathans_breath.so

$(PROG).dmg: $(PROG)
	mkdir tmp
	cp $(PROG) tmp
	hdiutil create -ov -volname "Raw HID Test" -srcfolder tmp $(PROG).dmg

hid.o: hid_$(OS).c hid.h
	$(CC) $(CFLAGS) -c -o $@ $<

driver.o: driver.cpp driver.hpp constants.hpp hid.o
	$(CXX) $(CXXFLAGS) -c -o $@ $<

python_binding.o: python_binding.cpp driver.o
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f *.o $(PROG) $(PROG).exe $(PROG).dmg $(PROG).so
	rm -rf tmp
