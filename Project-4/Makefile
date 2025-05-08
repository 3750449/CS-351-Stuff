#! make

CUDAFILES = $(wildcard *.cu)
CPPFILES = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)
TARGETS = $(CUDAFILES:.cu=.gpu) $(CPPFILES:.cpp=.cpu)

STD = -std=c++17
OPTIMIZER ?= -O2
CXXFLAGS = $(OPTIMIZER)

NVCC = nvcc
DIRT = $(wildcard *.i) $(wildcard *.ppm)

default all: $(TARGETS)

%.gpu : %.cu $(HEADERS)
	$(NVCC) $(CXXFLAGS) $< -o $@

%.cpu : %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	$(RM) $(DIRT)

rmtargets:
	$(RM) $(wildcard $(TARGETS))

clobber: clean rmtargets

.PHONY: default all clean rmtargets clobber