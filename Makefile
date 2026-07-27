CC = gcc

INCLUDE_DIR = include
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

TARGET = $(BIN_DIR)/all

CFLAGS = -Wall -Wextra -std=c17 -ggdb
DEPFLAGS = -MMD -MP
CPPFLAGS = -Iinclude

CFILES = $(wildcard $(SRC_DIR)/*.c)
OFILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(CFILES))

.PHONY: all all-run info dirs clean

all: dirs $(TARGET)

$(TARGET): $(OFILES)
	@$(CC) $(OFILES) $(CFLAGS) -o $(TARGET)

all-run: all run

dirs:
	@mkdir -p $(BIN_DIR) $(BUILD_DIR) $(SRC_DIR) $(INCLUDE_DIR)
	@mv $(wildcard *.c) $(SRC_DIR)/
	@mv $(wildcard *.h) $(INCLUDE_DIR)/

run:
	@./$(BIN_DIR)/all

run-%: $(BIN_DIR)/%
	@./$<

info:
	@echo "========================="
	@echo "          INFO           "
	@echo "========================="
	@echo
	@echo "CFLAGS:"
	@echo $(CFLAGS)
	@echo
	@echo "DEPFLAGS:"
	@echo $(DEPFLAGS)
	@echo
	@echo "CPPFLAGS:"
	@echo $(CPPFLAGS)
	@echo
	@echo "FILES:"
	@echo $(CFILES)

clean:
	@rm -rf $(BIN_DIR) $(BUILD_DIR)


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEPFLAGS) -c $< -o $@

-include $(OFILES:.o=.d)