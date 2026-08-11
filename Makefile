CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -O3 -g -fno-omit-frame-pointer

# Drapeaux pour les sanitizers
SAN_FLAGS = -fsanitize=address -fsanitize=undefined -fsanitize=leak

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

TARGET = $(BIN_DIR)/seg

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Avec sanitizers
all: CFLAGS += $(SAN_FLAGS)
all: LDFLAGS += $(SAN_FLAGS) -lm
all: directories $(TARGET)

# Sans sanitizers
release: clean
release: LDFLAGS += -lm
release: directories $(TARGET)

directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all release directories clean