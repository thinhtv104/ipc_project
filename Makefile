CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = 

# Danh sách chương trình cần build
all: server client test_client

# Build Server (Dev 3 main + Dev 1 IPC + Dev 2 Service)
server: src/server.o src/ipc_channel.o src/service_handler.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Build Client tương tác (Dev 4 main + Dev 1 IPC)
client: src/client.o src/ipc_channel.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Build Tool Test tải đa tiến trình (Dev 4 main + Dev 1 IPC)
test_client: src/test_client.o src/ipc_channel.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Quy tắc biên dịch file .c sang .o
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Lệnh dọn dẹp các file object và binary
clean:
	rm -f src/*.o server client test_client

.PHONY: all clean
