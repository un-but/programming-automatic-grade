CC = clang
CFLAGS = -Wall -Wextra -O2 -lm

SRC_DIR = src
BIN_DIR = bin

FILE ?=

.PHONY: build clean

$(BIN_DIR)/19_task: LDFLAGS = -ltomcrypt

$(BIN_DIR)/%: $(SRC_DIR)/%.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	@echo "✅ Успешно скомпилировано: $@"

clean:
	rm -rf $(BIN_DIR)
	@echo "🧹 Папка $(BIN_DIR) очищена"
