# Компилятор и флаги компиляции
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
LDFLAGS = -lncurses

# Директории
SRC_DIR = .
BRICK_GAME_DIR = $(SRC_DIR)/brick_game
TETRIS_DIR = $(BRICK_GAME_DIR)/tetris
GUI_DIR = $(SRC_DIR)/gui/cli
TEST_DIR = $(SRC_DIR)/tests
BUILD_DIR = $(SRC_DIR)/build

# Исходные файлы для основной программы
GAME_SRC = $(TETRIS_DIR)/backend.c $(TETRIS_DIR)/fsm.c $(TETRIS_DIR)/helpers.c $(TETRIS_DIR)/tetris.c $(GUI_DIR)/cli.c
GAME_OBJ = $(patsubst $(TETRIS_DIR)/%.c, $(BUILD_DIR)/%.o, $(GAME_SRC))

# Исполняемый файл основной программы
GAME_TARGET = tetris

# Исходные файлы для тестов
TEST_SRC = $(TEST_DIR)/test_all.c
TEST_TETRIS_SRC = $(TETRIS_DIR)/backend.c $(TETRIS_DIR)/fsm.c $(TETRIS_DIR)/helpers.c

# Объектные файлы тестов
TEST_OBJ = $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%.o, $(TEST_SRC))
TEST_TETRIS_OBJ = $(patsubst $(TETRIS_DIR)/%.c, $(BUILD_DIR)/test_%.o, $(TEST_TETRIS_SRC))

# Все объектные файлы для тестов
TEST_ALL_OBJ = $(TEST_OBJ) $(TEST_TETRIS_OBJ)

# Исполняемый файл теста
TEST_TARGET = test_all

# Директории для установки и документации
DESTDIR ?= /usr/local/bin
MANPAGE_DIR ?= /usr/local/share/man/man1
DOC_SRC = README.md

# Основная цель: компиляция основной программы
all: $(GAME_TARGET)

# Компиляция основной программы
$(GAME_TARGET): $(GAME_OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/$@ $(LDFLAGS)

# Компиляция объектных файлов для основной программы
$(BUILD_DIR)/%.o: $(TETRIS_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(GUI_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@	

# Компиляция объекта теста
$(BUILD_DIR)/test_all.o: $(TEST_DIR)/test_all.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -DUNIT_TESTING --coverage -c $< -o $@

# Компиляция объектных файлов tetris для тестов с покрытием
$(BUILD_DIR)/test_%.o: $(TETRIS_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -DUNIT_TESTING --coverage -c $< -o $@

# Запуск тестов
test: $(TEST_TARGET)
	@echo "Running unit tests..."
	./$(TEST_TARGET)

# Компиляция исполняемого файла теста
$(TEST_TARGET): $(TEST_ALL_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) -lgcov --coverage

# Отчет о покрытии кода
gcov_report: test
	@echo "Running GCOV to generate coverage data..."
	gcov -b -l -p -c -o $(BUILD_DIR) $(TETRIS_SRC) $(TEST_SRC)
	
	@echo "Generating HTML report with gcovr..."
	mkdir -p gcov_report
	gcovr -r . -o gcov_report/index.html --html-details

# Установка программы в DESTDIR
install: all
	@echo "Installing $(GAME_TARGET) to $(DESTDIR)..."
	@mkdir -p $(DESTDIR)
	cp $(BUILD_DIR)/$(GAME_TARGET) $(DESTDIR)

# Удаление установленной программы
uninstall:
	@echo "Uninstalling $(GAME_TARGET) from $(DESTDIR)..."
	rm -f $(DESTDIR)/$(GAME_TARGET)

# Очистка
clean:
	@echo "Cleaning up build artifacts..."
	@rm -rf $(BUILD_DIR) *.o *.a *.so *.gcda *.gcno gcov_report $(TEST_TARGET) $(GAME_TARGET) doc.dvi doc.log doc.aux

# Создание документации в формате DVI
dvi: doc.dvi

doc.dvi: doc.tex
	@echo "Creating documentation..."
	latex $<

# Создание архива дистрибутива
dist: clean
	@echo "Creating distribution archive..."
	mkdir $(GAME_TARGET)-dist
	cp -r $(SRC_DIR)/brick_game $(SRC_DIR)/tests $(SRC_DIR)/Makefile $(GAME_TARGET)-dist/
	tar -czvf $(GAME_TARGET).tar.gz $(GAME_TARGET)-dist
	rm -rf $(GAME_TARGET)-dist

# Основные цели
.PHONY: all clean test gcov_report install uninstall dvi dist