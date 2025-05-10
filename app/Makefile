OPZ_EXEC := opz

BUILD_DIR := $(shell pwd)/build
SRC_DIR := $(shell pwd)/src
INCLUDE_PATH := -I$(shell pwd)/include

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')

$(BUILD_DIR)/$(OPZ_EXEC): $(SRCS)
	$(CXX) -Wall $(INCLUDE_PATH) -o $@ $^