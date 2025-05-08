OPZ_EXEC := opz

BUILD_DIR := ./build
SRC_DIR := ./src
INCLUDE_PATH := -I/Users/atretyakov/Apps/simplex/include

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')

$(BUILD_DIR)/$(OPZ_EXEC): $(SRCS)
	$(CXX) -Wall $(INCLUDE_PATH) -o $@ $^