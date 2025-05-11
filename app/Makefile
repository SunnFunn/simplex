OPZ_EXEC := opz

BUILD_DIR := $(shell pwd)/build
SRC_DIR := $(shell pwd)/src
INCLUDE_PATH := -I$(shell pwd)/include
LIB_PATH := -L/opt/homebrew/Cellar/hiredis/1.3.0/lib
# LIB_PATH := -L/usr/local/lib/

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')

$(BUILD_DIR)/$(OPZ_EXEC): $(SRCS)
	$(CXX) -Wall $(INCLUDE_PATH) -o $@ $^ $(LIB_PATH) -lhiredis