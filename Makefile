# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -fPIC -Wall -O2

# Python and pybind11 configuration
PYTHON_VERSION = 3.13
PYTHON_CONFIG = python$(PYTHON_VERSION)-config
PYTHON_INCLUDES = $(shell python3 -m pybind11 --includes)
PYTHON_LDFLAGS = $(shell $(PYTHON_CONFIG) --ldflags --embed 2>/dev/null || $(PYTHON_CONFIG) --ldflags)
PYTHON_EXTENSION = $(shell python3-config --extension-suffix)

# Directories
SRC_DIR = src
BUILD_DIR = build

# Source files
SOURCES = $(SRC_DIR)/py_interface.cpp
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Target shared library
TARGET = py_interface$(PYTHON_EXTENSION)

# Default target
all: $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(PYTHON_INCLUDES) -c $< -o $@

# Link shared library
$(TARGET): $(OBJECTS)
	$(CXX) -shared $(OBJECTS) $(PYTHON_LDFLAGS) -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Phony targets
.PHONY: all clean
