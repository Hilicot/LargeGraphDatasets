CXX := g++
CXXFLAGS := -O3 -march=native
all: dataset

dataset: process_dataset.cpp Graph.cpp Graph.h
	$(CXX) $(CXXFLAGS) -Wall -std=c++2a -o prog process_dataset.cpp Graph.cpp
	