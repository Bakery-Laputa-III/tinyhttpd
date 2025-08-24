CC = clang
CFLAGS = -std=c99 -Wall -Wextra -pedantic -pthread -O2
TARGET = tinyhttpd
SRCS = tinyhttpd.c
BUILD_DIR = build
BIN_DIR = bin
OBJS = $(SRCS:.c=.o)
OBJS_WITH_PATH = $(addprefix $(BUILD_DIR)/, $(OBJS))
TARGET_WITH_PATH = $(BIN_DIR)/$(TARGET)

.PHONY: all clean run install htdocs test

all: $(TARGET_WITH_PATH)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET_WITH_PATH): $(OBJS_WITH_PATH) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

run: $(TARGET_WITH_PATH)
	./$(TARGET_WITH_PATH)

install: $(TARGET_WITH_PATH)
	sudo cp $(TARGET_WITH_PATH) /usr/local/bin/

htdocs:
	mkdir -p htdocs
	echo '<!DOCTYPE html><html><head><title>Welcome to tinyhttpd</title></head><body><h1>Welcome to tinyhttpd!</h1><p>This is a minimal HTTP server written in C.</p></body></html>' > htdocs/index.html

test: htdocs $(TARGET_WITH_PATH)
	./$(TARGET_WITH_PATH) &
	sleep 2
	curl -s http://localhost:8080/ | grep -q "欢迎使用"
	@echo "Test passed!"
	pkill -f $(TARGET)

debug: CFLAGS += -g -DDEBUG
debug: $(TARGET_WITH_PATH)

release: CFLAGS += -O3 -DNDEBUG
release: $(TARGET_WITH_PATH)