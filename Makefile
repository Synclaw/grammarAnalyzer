# 编译器及编译选项
CXX       := g++
CXXFLAGS  := -Wall -std=c++17 -g -Iinclude

# 目标可执行文件名称
TARGET    := parser

# 目录结构
SRC_DIR   := src
BUILD_DIR := build

# 源文件列表（在 src/ 下）
SOURCES   := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS   := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

.PHONY: all clean

# 默认目标：生成可执行文件
all: $(BUILD_DIR) $(TARGET)

# 创建构建目录
$(BUILD_DIR):
	@mkdir -p $@

# 链接可执行文件
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 编译规则：将 .cpp 文件编译到 build/ 下的 .o 文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清除编译生成的文件
clean:
	rm -rf $(BUILD_DIR) $(TARGET)