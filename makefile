# --- Toolchain ---
CXX_GNU   := g++
CXX_CLANG := clang++

# --- Project layout ---
SRC_DIR   := src
INC_DIR   := include
BUILD_DIR := build

TARGET    := app
TARGET_ASAN := app_asan

SRCS := $(SRC_DIR)/main.cpp $(SRC_DIR)/person.cpp
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# --- Flags ---
CXXFLAGS := -std=c++20 -g -O0 -Wall -Wextra -pedantic -I$(INC_DIR)
LDFLAGS  :=

SANFLAGS := -fsanitize=address,undefined -fno-omit-frame-pointer

# --- Phony targets ---
.PHONY: all run asan run-asan valgrind doctor clean

all: $(TARGET)

# Ensure build dir exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile .cpp -> .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX_GNU) $(CXXFLAGS) -c $< -o $@

# Link
$(TARGET): $(OBJS)
	$(CXX_GNU) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

# ASan build (link with clang)
asan: $(SRCS)
	$(CXX_CLANG) $(CXXFLAGS) $(SANFLAGS) $(SRCS) -o $(TARGET_ASAN) $(LDFLAGS)

run-asan: asan
	ASAN_SYMBOLIZER_PATH=$$(command -v llvm-symbolizer) \
	ASAN_OPTIONS=symbolize=1 \
	./$(TARGET_ASAN)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

doctor:
	@echo "== Toolchain doctor =="
	@echo "--- g++"; g++ --version | head -n 1
	@echo "--- clang++"; clang++ --version | head -n 1
	@echo "--- make"; make --version | head -n 1
	@echo "--- valgrind"; valgrind --version | head -n 1
	@echo "--- llvm-symbolizer"; (llvm-symbolizer --version | head -n 1) || echo "llvm-symbolizer not found"
	@echo "--- gdb"; gdb --version | head -n 1
	@echo "--- git"; git --version
	@echo "--- gh (optional)"; (gh --version | head -n 1) || echo "gh not installed"

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TARGET_ASAN)