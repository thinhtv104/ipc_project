CC = gcc
CFLAGS = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -O2 -Iinclude
LDLIBS = -lm
LDFLAGS = 

# Danh sách chương trình cần build
all: server client test_client gui_client

# Build Server (Dev 3 main + Dev 1 IPC + Dev 2 Service)
server: src/server.o src/ipc_channel.o src/service_handler.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

# Build Client tương tác (Dev 4 main + Dev 1 IPC)
client: src/client.o src/ipc_channel.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

# Build Tool Test tải đa tiến trình (Dev 4 main + Dev 1 IPC)
test_client: src/test_client.o src/ipc_channel.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

# Quy tắc biên dịch file .c sang .o
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Lệnh dọn dẹp các file object và binary
clean:
	rm -f src/*.o server client test_client service_tests gui_client

.PHONY: all clean

service_tests: tests/service_tests.c src/service_handler.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

test: all service_tests
	./service_tests
	bash tests/integration.sh

.PHONY: test

gui_client: src/gui_client.o src/ipc_channel.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

gui: all
	python3 gui.py

.PHONY: gui
