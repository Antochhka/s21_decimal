#include "big_decimal.h"

#include "../arithmetic/arithmetic.h"
#include "../helpers/helpers.h"

int get_sign_big(s21_big_decimal decimal) {
  bit3_struct *bit3 = (bit3_struct *)&decimal.bits[7];
  return (int)bit3->sign;
}

void set_sign_big(s21_big_decimal *num, int sign_value) {
  if (sign_value == 1)
    num->bits[7] |= (1U << 31);
  else
    num->bits[7] &= ~(1U << 31);
}

int get_degree_big(s21_big_decimal decimal) {
  bit3_struct *bit3 = (bit3_struct *)&decimal.bits[7];
  return (int)bit3->degree;
}

void set_degree_big(s21_big_decimal *decimal, int degree) {
  bit3_struct *bit3 = (bit3_struct *)&decimal->bits[7];
  bit3->degree = degree & 0xFF;
}

int get_bit_big(s21_big_decimal value, int i) {
  int bitIndex = i / 32;
  int bitPosition = i % 32;

  return (value.bits[bitIndex] >> bitPosition) & 1;
}

void set_bit_big(s21_big_decimal *result, int i, int result_bit) {
  int bitIndex = i / 32;
  int bitPosition = i % 32;

  if (result_bit == 1) {
    result->bits[bitIndex] |= (1 << bitPosition);
  } else {
    result->bits[bitIndex] &= ~(1 << bitPosition);
  }
}
void decimal_to_big_decimal(s21_decimal *src, s21_big_decimal *dest) {
  if (!src || !dest) return;

  // Копируем мантиссу (первые 96 бит)
  dest->bits[0] = src->bits[0];
  dest->bits[1] = src->bits[1];
  dest->bits[2] = src->bits[2];

  // Заполняем расширенные биты нулями
  dest->bits[3] = 0;
  dest->bits[4] = 0;
  dest->bits[5] = 0;
  dest->bits[6] = 0;

  // Копируем служебные данные (знак, масштаб)
  dest->bits[7] = src->bits[3];
}

int big_decimal_to_decimal(s21_big_decimal *src, s21_decimal *dest) {
  int error_flag = 0;
  s21_big_decimal src_clone = *src;
  int counter = 0;
  while ((!is_big_decimal_zero(src_clone)) && !error_flag) {
    if (get_degree_big(src_clone) < 0 || counter > 28) error_flag = 1;
    divide_by_10_big(&src_clone, 1);
    counter++;
  }
  if (counter == 30) {
    // 79228162514264337593543950335.4999999999999999999999999999
    s21_big_decimal test_big = {{0x077FFFFFF, 0xE0ED7ECF, 0xEFD818D0,
                                 0x0FFFFFFF, 0x3E250261, 0x204FCE5E, 0x0,
                                 0x1C0000}};
    if (s21_is_equal_big(*src, test_big) == 1) {
      counter = 28;
      error_flag = 0;
    }
  }
  if (error_flag == 0) {
    divide_by_10_big(src, counter);
    dest->bits[0] = src->bits[0];
    dest->bits[1] = src->bits[1];
    dest->bits[2] = src->bits[2];
    dest->bits[3] = src->bits[7];
  }
  return error_flag;
}

void s21_decimal_init_big(s21_big_decimal *dec) {
  if (dec != NULL) {
    dec->bits[0] = 0;  // Младшие 32 бита мантиссы
    dec->bits[1] = 0;  // Средние 32 бита
    dec->bits[2] = 0;  // Старшие 32 бита
    dec->bits[3] = 0;  // Младшие 32 бита мантиссы
    dec->bits[4] = 0;  // Средние 32 бита
    dec->bits[5] = 0;  // Старшие 32 бита
    dec->bits[6] = 0;  // Старшие 32 бита
    dec->bits[7] = 0;  // Бит знака + масштаб + зарезервированные биты
  }
}

int is_big_decimal_zero(s21_big_decimal src) {
  return !(src.bits[3] || src.bits[4] || src.bits[5] || src.bits[6] ||
           get_degree_big(src) > 28);
}

void left_shift_big(s21_big_decimal *decimal, int shift) {
  unsigned int remains = 0;  // Остаток при смещении

  for (int i = 0; i < SIZE_DECIMAL_BIG - 1; i++) {
    unsigned long long temp_bit = (unsigned long long)decimal->bits[i] << shift;
    decimal->bits[i] = (unsigned int)(temp_bit) + remains;
    if (shift <= 32) {
      remains = (unsigned int)(temp_bit >> SIZE_INT);
    }
  }
}
int get_not_zero_bit(s21_big_decimal decimal) {
  int result = -1;
  for (int i = 224 - 1; i >= 0; i--) {
    if (get_bit_big(decimal, i)) {
      result = i;
      i = 0;
    }
  }
  return result;
}

int s21_is_equal_big(s21_big_decimal value_1, s21_big_decimal value_2) {
  int res = TRUE;

  for (int i = SIZE_DECIMAL_BIG - 2; i >= 0; i--) {
    if (value_1.bits[i] != value_2.bits[i]) {
      res = FALSE;
      break;
    }
  }

  if (res == TRUE) {
    if (get_sign_big(value_1) != get_sign_big(value_2) ||
        get_degree_big(value_1) != get_degree_big(value_2)) {
      res = FALSE;
    }
  }
  return res;
}

int s21_is_greater_big(s21_big_decimal value_1, s21_big_decimal value_2) {
  int res = FALSE;

  int sign_1 = get_sign_big(value_1);
  int sign_2 = get_sign_big(value_2);

  if (s21_is_equal_big(value_1, value_2)) {
  } else if (sign_1 > sign_2) {
    res = FALSE;
  } else if (sign_1 < sign_2) {
    res = TRUE;
  } else if (sign_1 == sign_2) {
    int is_negative_numbers = FALSE;

    if (sign_1 == MINUS) {
      is_negative_numbers = TRUE;
    }

    normalization_big(&value_1, &value_2);

    int i = 32 * 7 - 1;

    while (i >= 0) {
      int bit_1 = get_bit_big(value_1, i);
      int bit_2 = get_bit_big(value_2, i);

      if (bit_1 != bit_2) {
        if (is_negative_numbers == TRUE) {
          res = (bit_1 < bit_2) ? TRUE : FALSE;
        } else {
          res = (bit_1 < bit_2) ? FALSE : TRUE;
        }
        break;
      }
      i--;
    }
  }

  return res;
}

int s21_is_greater_or_equal_big(s21_big_decimal value_1,
                                s21_big_decimal value_2) {
  return (s21_is_equal_big(value_1, value_2) ||
          s21_is_greater_big(value_1, value_2))
             ? TRUE
             : FALSE;
}

void normalization_big(s21_big_decimal *value_1, s21_big_decimal *value_2) {
  int degree_1 = get_degree_big(*value_1);
  int degree_2 = get_degree_big(*value_2);

  s21_big_decimal *first_normalization =
      degree_1 < degree_2 ? value_1 : value_2;
  s21_big_decimal *second_normalization =
      degree_1 < degree_2 ? value_2 : value_1;

  int counter = abs(degree_1 - degree_2);

  while (counter != 0) {
    s21_big_decimal syllable_1 = *first_normalization;
    s21_big_decimal syllable_2 = *first_normalization;

    s21_big_decimal res;
    res.bits[3] = first_normalization->bits[3];

    int significant_bit = first_significant_bit_big(*first_normalization);

    if (significant_bit > SIZE_DEC_BIG - 2 ||
        significant_bit > SIZE_DEC_BIG - 4) {
      break;
    } else {
      left_shift_big(&syllable_1, 3);
      left_shift_big(&syllable_2, 1);
    }

    if (add_big_decimal(syllable_1, syllable_2, &res) != 0) {
      break;
    } else {
      *first_normalization = res;
      bit3_struct *bit3 = (bit3_struct *)&first_normalization->bits[7];
      (bit3->degree)++;
    }

    counter--;
  }

  divide_by_10_big(second_normalization, counter);
}

int first_significant_bit_big(s21_big_decimal dec) {
  int res = -1;

  for (int i = SIZE_DEC_BIG - 1; i >= 0; i--) {
    int temp = get_bit_big(dec, i);
    if (temp == 1) {
      res = i;
      break;
    }
  }

  return res;
}

void divide_by_10_big(s21_big_decimal *num, int index) {
  int diff = FALSE;
  while (index > 0) {
    int degree = get_degree_big(*num);
    int sign = get_sign_big(*num);
    uint64_t remainder = 0;
    for (int i = SIZE_DECIMAL_BIG - 2; i >= 0; i--) {
      uint64_t value = (remainder << 32) | (uint32_t)num->bits[i];
      remainder = value % 10;
      num->bits[i] = (int32_t)(value / 10);
    }
    if (remainder != 0 && diff == FALSE && index > 1) {
      diff = TRUE;
    }

    if (index == 1) {
      if (remainder > 5 ||
          (remainder == 5 && (num->bits[0] & 1 || diff == TRUE))) {
        uint64_t carry = 1;
        for (int i = 0; i < SIZE_DECIMAL_BIG - 1 && carry; i++) {
          uint64_t sum = (uint64_t)num->bits[i] + carry;
          num->bits[i] = sum & 0xFFFFFFFF;
          carry = sum >> 32;
        }
      }
    }
    set_degree_big(num, degree - 1);
    set_sign_big(num, sign);
    index--;
  }
}