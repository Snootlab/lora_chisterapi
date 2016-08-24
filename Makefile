TARGET=chisterapi

CC=gcc
CXX=g++
RM=rm -f

INCLUDES_PATH=-I lib/radiohead/
CPPFLAGS=-std=c++11 -g -Wall -DRH_PLATFORM=RH_PLATFORM_RPI -D__RASPBERRY_PI_ $(INCLUDES_PATH)
CFLAGS=-g -Wall -DRH_PLATFORM=RH_PLATFORM_RPI -D__RASPBERRY_PI_ $(INCLUDES_PATH)
LDFLAGS=-lwiringPi

RH95_SRCS=lib/radiohead/RH_RF95.cpp \
	lib/radiohead/RHGenericSPI.cpp \
	lib/radiohead/RHLinuxSPI.cpp \
 	lib/radiohead/RHGenericDriver.cpp
RH95_OBJS=$(subst .cpp,.o,$(RH95_SRCS))

all: $(TARGET)

$(TARGET): src/main.o $(RH95_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CPPFLAGS) -o $@ -c $<
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	$(RM) -f $(TARGET) $(RH95_OBJS) src/main.o
