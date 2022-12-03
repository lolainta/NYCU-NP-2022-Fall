TARGET_EXEC := server client

BUILD_DIR := ./build
SRC_DIR := ./src
TEST_DIR := ./test
TEST_SRC_DIR := $(TEST_DIR)/src
TEST_DEST_DIR := $(TEST_DIR)/dest

_SRCS := server.cpp client.cpp
SRCS := $(patsubst %.cpp,$(SRC_DIR)/%.cpp,$(_SRCS))

CXX=g++
CPPFLAGS=-O3 -g -Wall -std=c++17 -static


.PHONY: all clean check

all: 
	@mkdir -p $(BUILD_DIR)
	make $(BUILD_DIR)/server
	make $(BUILD_DIR)/client


$(BUILD_DIR)/server: $(SRC_DIR)/server.cpp
	$(CXX) $(SRC_DIR)/server.cpp $(CPPFLAGS) -o $(BUILD_DIR)/server

$(BUILD_DIR)/client: $(SRC_DIR)/client.cpp
	$(CXX) $(SRC_DIR)/client.cpp $(CPPFLAGS) -o $(BUILD_DIR)/client

check:
	@md5sum $(TEST_SRC_DIR)/* | cut -d ' ' -f 1 > $(BUILD_DIR)/src.txt
	@md5sum $(TEST_DEST_DIR)/* | cut -d ' ' -f 1 > $(BUILD_DIR)/dest.txt
	diff $(BUILD_DIR)/src.txt $(BUILD_DIR)/dest.txt
	@rm $(BUILD_DIR)/src.txt $(BUILD_DIR)/dest.txt

clean:
	rm -r $(BUILD_DIR)
	rm -rf $(TEST_DEST_DIR)

