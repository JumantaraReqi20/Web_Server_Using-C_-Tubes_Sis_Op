# Nama executable
TARGET = server

# Compiler
CC = gcc

# Opsi kompilasi
CFLAGS = -Wall -Wextra -g

# Daftar file sumber
SRCS = main.c server.c rate_limit.c logger.c file_handler.c

# Daftar file objek
OBJS_DIR = obj
OBJS = $(patsubst %.c,$(OBJS_DIR)/%.o,$(SRCS))

# Aturan untuk membangun executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Aturan untuk membangun file objek
$(OBJS_DIR)/%.o: %.c | $(OBJS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Aturan untuk membuat direktori objek
$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

# Aturan untuk membersihkan file objek dan executable
clean:
	rm -rf $(OBJS_DIR) $(TARGET)

# Aturan untuk menjalankan server
run: $(TARGET)
	./$(TARGET)

.PHONY: clean run