# Compilador y banderas (flags)
CC = gcc
# -Wall y -Wextra muestran advertencias útiles. -g activa los símbolos para depurar con Valgrind/strace
CFLAGS = -Wall -Wextra -g -I./include -fsanitize=address

# Directorios
SRC_DIR = src
BUILD_DIR = build
TARGET = $(BUILD_DIR)/editor

# Buscar todos los archivos .c en src/
SRCS = $(wildcard $(SRC_DIR)/*.c)
# Reemplazar .c por .o para los archivos objeto
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Regla principal
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Regla para compilar los .c en .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Regla para limpiar el proyecto (make clean)
clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: clean