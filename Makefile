#! /usr/bin/env make

CXXFILES = $(wildcard *.cpp)
PROGRAMS = $(CXXFILES:.cpp=)

DATA_PATH ?= "\"/home/faculty/shreiner/public_html/CS-351/Project-1/Data\""
DEFINES = -DDATA_PATH=$(DATA_PATH)

OPT ?= -g
CXXFLAGS = $(DEFINES) $(OPT) -std=c++20 $(FLAGS)

DIRT = $(wildcard hash-*.txt)

default: $(PROGRAMS)

%: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	$(RM) $(DIRT)

rmtargets:
	$(RM) $(PROGRAMS)

clobber: clean rmtargets

.PHONY: default clean rmtargets clobber
