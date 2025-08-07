CXX := g++
CXXFLAGS := -std=c++20 -Wall -g
LDFLAGS :=

INCLUDE_DIR := include
SRC_DIR := src
OBJ_DIR := obj

CXXFLAGS += $(shell pkg-config --cflags gmp jsoncpp) -I$(INCLUDE_DIR)
LDFLAGS += $(shell pkg-config --libs gmp jsoncpp)

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

TARGET := proxy_svr

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Build finished: $(TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Cleaning up..."
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean