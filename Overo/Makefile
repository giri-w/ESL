#
# Folder definitions
# 
PWD:=$(shell pwd)
SOURCE_DIR:=$(PWD)/src
INCLUDE_DIR:=$(PWD)/include
BUILD_DIR:=$(PWD)/build
OBJECT_DIR=$(BUILD_DIR)/obj
KERNEL_MODULE_DIR=$(PWD)/kernel

#
# Source files
#
SOURCE_FILES_CPP=$(SOURCE_DIR)/mainSetup.cpp $(SOURCE_DIR)/gpmc_driver_cpp.cpp $(SOURCE_DIR)/objectTrack_cpp.cpp

#
# Object files
#
OBJECT_FILES_CPP=$(patsubst %.cpp,%.o,$(patsubst $(SOURCE_DIR)/%, $(OBJECT_DIR)/%, $(SOURCE_FILES_CPP)))

#
# Commands
#
#CROSS_COMPILE=arm-linux-gnueabi-
CROSS_COMPILE=
CXX=$(CROSS_COMPILE)g++
#CC=$(CROSS_COMPILE)gcc
RM=rm -f
MKDIR=mkdir -p

#
# Flags
# You can define your own libraries and flags here.
#
LIBS=
LDFLAGS=
CFLAGS=-I$(INCLUDE_DIR) -I$(KERNEL_MODULE_DIR)/include -Wall
CPPFLAGS=
CXXFLAGS=$(shell pkg-config --cflags opencv)
CXXLIBS=$(shell pkg-config --libs opencv)




#
# Targets
#
.PHONY: all c cpp clean
all: cpp c

# Compile .cpp files to .o (object) files.
$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	$(MKDIR) $(BUILD_DIR) $(OBJECT_DIR)
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CXXLIBS) $(CPPFLAGS) -o $@ $<

# Compile c++ example
cpp: $(OBJECT_FILES_CPP)
	$(CXX) $(LDFLAGS) $^ $(LIBS) $(CXXFLAGS) $(CXXLIBS) -o gpmc_example_$@

# Remove build directory and compiled applications.
clean:
	$(RM) gpmc_example_c gpmc_example_cpp
	$(RM) -r $(BUILD_DIR)
