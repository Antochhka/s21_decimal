#ifndef SRC_S21_DECIMAL_CONVERSION_CONVERSION_H_
#define SRC_S21_DECIMAL_CONVERSION_CONVERSION_H_

#include "../types/types.h"

#define MIN_FLOAT_TO_CONVERT 1e-28
#define MAX_FLOAT_TO_CONVERT 79228157791897854723898736640.0f

typedef enum {
  S21_CONVERSION_OK = 0,
  S21_CONVERSION_ERROR = 1
} s21_conversion_result;

typedef union {
  float f;
  struct {
    unsigned mantissa : 23;
    unsigned exp : 8;
    unsigned sign : 1;
  } fields;
} FloatBits;

// Преобразование из int в decimal
int s21_from_int_to_decimal(int src, s21_decimal *dst);

// Преобразование из float в decimal
int s21_from_float_to_decimal(float src, s21_decimal *dst);

// Преобразование из decimal в int
int s21_from_decimal_to_int(s21_decimal src, int *dst);

// Преобразование из decimal в float
int s21_from_decimal_to_float(s21_decimal src, float *dst);

#endif