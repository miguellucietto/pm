CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -std=c17 -ggdb -Iinclude -MMD -MP

SRC_DIR = src
BUILD_DIR = build

CFILES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(CFILES))
DEPS = $(OBJECTS:.o=.d)

TARGET = pm


dev: all
	@sudo install -Dm755 ./$(TARGET) /usr/local/bin/pm

undev:
	@sudo rm -f /usr/local/bin/pm


all: $(TARGET) 


$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR) $(TARGET) compile_commands.json

bear:
	@bear -- $(MAKE)

-include $(DEPS)
