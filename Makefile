CXX = g++
CXXFLAGS = -g -Wall -Wextra -O3 -std=c++11
LDFLAGS = -W

INCLUDE_DIR = include
SRC_DIR = src
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

TARGET = log_kv

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

-include $(OBJS:.o=.d)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)


