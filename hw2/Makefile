BUILD_DIR := ./build
SRC_DIR := ./src

CONFIG_DIR := ./configs
CONFIGS := $(CONFIG_DIR)/config.txt

_OBJS := UDPSocket.o DNSSocket.o message.o config.o
OBJS := $(patsubst %.o,$(BUILD_DIR)/%.o,$(_OBJS))

CXX := g++
CPPFLAGS := -Ofast -g -Wall -std=c++20 -static 

.PHONY: all clean run

all:
	@mkdir -p $(BUILD_DIR)
	make $(BUILD_DIR)/dns

$(BUILD_DIR)/dns: $(SRC_DIR)/dns.cpp $(OBJS)
	$(CXX) $< $(OBJS) $(CPPFLAGS) -o $@


$(BUILD_DIR)/config.o: $(SRC_DIR)/config.cpp $(SRC_DIR)/config.h
	$(CXX) $(CPPFLAGS) -DCONFIG_DIR='"$(CONFIG_DIR)/"' -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) -c $< -o $@

run: $(BUILD_DIR)/dns $(CONFIGS)
	$< 1053 $(CONFIGS)
    
clean:
	rm -r $(BUILD_DIR)
	rm -rf $(TEST_DEST_DIR)
