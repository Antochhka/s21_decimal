# Компилятор и флаги
OS = $(shell uname)
CC = gcc
CFLAGS = -std=c11
GCOV_FLAGS = -fprofile-arcs -ftest-coverage
AR = ar
ARFLAGS = rcs

ifeq ($(OS), Linux)
	LDFLAGS = -lcheck -lm -lsubunit
else
  LDFLAGS = -lcheck
endif

# Исходные файлы
ARITHMETIC_SRC = $(wildcard arithmetic/*.c)
BIG_DECIMAL_SRC = $(wildcard big_decimal/*.c)
COMPARISON_SRC = $(wildcard comparison/*.c)
CONVERSION_SRC = $(wildcard conversion/*.c)
OTHER_SRC = $(wildcard other/*.c)
HELPERS_SRC = $(wildcard helpers/*.c)

ALL_SRC = $(ARITHMETIC_SRC) \
      		$(BIG_DECIMAL_SRC) \
			$(COMPARISON_SRC) \
			$(CONVERSION_SRC) \
			$(OTHER_SRC) \
			$(HELPERS_SRC)

# Директория с объектными файлами
OBJ_DIR = src

# Объектные файлы
OBJ = $(notdir $(ARITHMETIC_SRC:.c=.o)) \
      $(notdir $(BIG_DECIMAL_SRC:.c=.o)) \
      $(notdir $(COMPARISON_SRC:.c=.o)) \
      $(notdir $(CONVERSION_SRC:.c=.o)) \
      $(notdir $(OTHER_SRC:.c=.o)) \
      $(notdir $(HELPERS_SRC:.c=.o))

# Тестовые файлы
TEST_SRC = test.c
TEST_OBJ = $(TEST_SRC:.c=.o)

# Имя библиотеки
LIB_NAME = s21_decimal.a

# Ниже все цели
.PHONY: all clean test gcov_report

all: $(LIB_NAME) test

rebuild: clean all

# Сборка статической библиотеки
$(LIB_NAME): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

# Правила для создания объектных файлов
%.o: arithmetic/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: big_decimal/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: comparison/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: conversion/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: other/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: helpers/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Сборка и запуск тестов
test: $(LIB_NAME) $(TEST_OBJ)
	$(CC) $(CFLAGS) -o test $(TEST_OBJ) -L. s21_decimal.a $(LDFLAGS)
	./test

valgrind_check: test
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file="valgrind.log"  ./test

leaks_check: test
	leaks -atExit -- ./test

# Формирование отчёта gcov в виде html-страницы
gcov_report: test
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(TEST_SRC) $(ALL_SRC) -o gcov_report_test $(LDFLAGS)
	./gcov_report_test
	lcov --capture --directory . --output-file coverage.info --exclude 'test.c'
	genhtml coverage.info --output-directory gcov_report

format:
	find . -name "*.c" -exec clang-format -i {} \;

clean:
	rm -f *.o *.a *.gcno *.gcda test gcov_report_test
	rm -rf gcov_report coverage.info
