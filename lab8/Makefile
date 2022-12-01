TARGET_EXEC := server client

BUILD_DIR := ./build
SRC_DIR := ./src

_SRCS := server.cpp client.cpp
SRCS := $(patsubst %.cpp,$(SRC_DIR)/%.cpp,$(_SRCS))

CXX=g++
CPPFLAGS=-O3 -g -Wall -std=c++20


.PHONY: all clean

all: 
	@mkdir -p $(BUILD_DIR)
	g++ $(SRC_DIR)/client.cpp -o $(BUILD_DIR)/client
	g++ $(SRC_DIR)/server.cpp -o $(BUILD_DIR)/server

clean:
	rm -r $(BUILD_DIR)

