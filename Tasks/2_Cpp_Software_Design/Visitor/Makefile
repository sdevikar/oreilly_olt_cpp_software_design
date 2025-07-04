#==================================================================================================
#
#  Makefile for subchapter "Visitor" of chapter "C++ Software Design"
#
#  Copyright (C) 2015-2025 Klaus Iglberger - All Rights Reserved
#
#  This file is part of the C++ training by Klaus Iglberger. The file may only be used in the
#  context of the C++ training or with explicit agreement by Klaus Iglberger.
#
#==================================================================================================


# Compiler settings
CXX = g++
CXXFLAGS = -std=c++20 -Wall


# Setting the source and binary files
SRC = $(wildcard *.cpp)
BIN = $(SRC:.cpp=)


# Rules
default: AcyclicVisitor Variant Visitor_Benchmark Visitor_Classic Visitor_Refactoring

AcyclicVisitor: AcyclicVisitor.cpp
	$(CXX) $(CXXFLAGS) -o AcyclicVisitor AcyclicVisitor.cpp

Variant: Variant.cpp
	$(CXX) $(CXXFLAGS) -o Variant Variant.cpp

Visitor_Benchmark: Visitor_Benchmark.cpp
	$(CXX) $(CXXFLAGS) -o Visitor_Benchmark Visitor_Benchmark.cpp

Visitor_Classic: Visitor_Classic.cpp
	$(CXX) $(CXXFLAGS) -o Visitor_Classic Visitor_Classic.cpp

Visitor_Refactoring: Visitor_Refactoring.cpp
	$(CXX) $(CXXFLAGS) -o Visitor_Refactoring Visitor_Refactoring.cpp

clean:
	@$(RM) $(BIN)


# Setting the independent commands
.PHONY: default clean
