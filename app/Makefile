OPZ_EXEC := opz

BUILD_DIR := ./build
SRC_DIR := ./src
INCLUDE_PATH := -I$(shell pwd)/include

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')

$(BUILD_DIR)/$(OPZ_EXEC): $(SRCS)
	$(CXX) -Wall $(INCLUDE_PATH) -o $@ $^