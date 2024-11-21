# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude -MMD
TARGET = main

# Исходные файлы и директории
SRC_DIR = src
OBJ_DIR = obj

# Автоматический поиск .c файлов
SRC = $(wildcard $(SRC_DIR)/**/*.c) $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
DEPS = $(OBJ:.o=.d)

# Правила
.PHONY: all clean

all: $(TARGET)

# Сборка основного исполняемого файла
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Правило для компиляции .c -> .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Учитываем зависимости
-include $(DEPS)

# Очистка
clean:
	rm -rf $(OBJ_DIR) $(TARGET)
