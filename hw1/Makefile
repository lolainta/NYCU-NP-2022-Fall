TARGET_EXEC := server

BUILD_DIR := ./build
SRC_DIR := ./src

_OBJS := main.o client.o channel.o response.o
OBJS := $(patsubst %.o,$(BUILD_DIR)/%.o,$(_OBJS))

CXX=g++
CPPFLAGS=-O3 -g -Wall -std=c++17


$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) -c $< -o $@

.PHONY: all clean

test:
	echo $(OBJS)

all: server

clean:
	rm -r $(BUILD_DIR)

