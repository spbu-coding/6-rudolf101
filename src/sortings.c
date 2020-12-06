#include "sortings.h"
#include <stdlib.h>

#define ASCII_SIZE 256

void swap(char **str1, char **str2){
    char *temp = *str1;
    *str1 = *str2;
    *str2 = temp;
}

void bubble(strings_array_t strings_array, array_size_t number_of_rows_to_sort, comparator_func_t cmp){
    for (array_size_t i = 0; i + 1 < number_of_rows_to_sort; ++i)
        for (array_size_t j = 0; j + 1 < number_of_rows_to_sort; ++j)
            if (cmp(strings_array[j], strings_array[j + 1]) > 0)
                swap(&strings_array[j], &strings_array[j + 1]);
}

void insertion(strings_array_t strings_array, array_size_t number_of_rows_to_sort, comparator_func_t cmp){
    for (array_size_t i = 0; i + 1 < number_of_rows_to_sort; ++i){
        array_size_t cur_index = i;
        for(array_size_t j = i + 1; j < number_of_rows_to_sort; ++j){
            if(cmp(strings_array[cur_index], strings_array[j]) > 0)
                cur_index = j;
        }
        if(cur_index != i)
            swap(&strings_array[i], &strings_array[cur_index]);
    }
}

void merge(strings_array_t strings_array, array_size_t number_of_rows_to_sort, comparator_func_t cmp){
    // Iterative algorithm inspired by neerc.ifmo.ru
    char **tmp = (char **) malloc(sizeof(char *) * number_of_rows_to_sort);
    for (array_size_t i = 1; i < number_of_rows_to_sort; i *= 2)
        for (array_size_t j = 0; j < number_of_rows_to_sort; j += i * 2) {

            array_size_t left;
            if(number_of_rows_to_sort - j < i)
                left = number_of_rows_to_sort - j;
            else
                left = i;

            array_size_t right;
            if (j + i >= number_of_rows_to_sort)
                right = 0;
            else if(number_of_rows_to_sort - j - i < i)
                right = number_of_rows_to_sort - j - i;
            else
                right = i;

            array_size_t it1 = 0;
            array_size_t it2 = 0;

            while (it1 < left && it2 < right) {
                if (cmp(strings_array[j + it1], strings_array[j + i + it2]) < 0) {
                    swap(&tmp[it1 + it2], &strings_array[j + it1]);
                    ++it1;
                } else {
                    swap(&tmp[it1 + it2], &strings_array[j + i + it2]);
                    ++it2;
                }
            }
            while (it1 < left) {
                swap(&tmp[it1 + it2], &strings_array[j + it1]);
                ++it1;
            }
            while (it2 < right) {
                swap(&tmp[it1 + it2], &strings_array[j + i + it2]);
                ++it2;
            }
            for (array_size_t count = 0; count < i * 2; count++) {
                if (j + count >= number_of_rows_to_sort)
                    break;
                swap(&tmp[count], &strings_array[j + count]);
            }
    }
    free(tmp);
}

void quick(strings_array_t strings_array, array_size_t number_of_rows_to_sort, comparator_func_t cmp) {
    qsort(strings_array, number_of_rows_to_sort, sizeof(char *), (int (*)(const void*, const void*)) cmp);
}

void radix(strings_array_t strings_array, array_size_t number_of_rows_to_sort, comparator_func_t cmp){
    // @zishkaz helped me figure it out
    array_size_t max_len = strlen(strings_array[0]);
    for (array_size_t i = 1; i < number_of_rows_to_sort; ++i)
        if (strlen(strings_array[i]) > max_len) max_len = strlen(strings_array[i]);

    int radix_num = -1;
    for (array_size_t j = 0; j < max_len; j++) {
        array_size_t i = 0;
        while (strlen(strings_array[i]) <= j) ++i;
        char first = strings_array[0][j];
        for (i = 0; i < number_of_rows_to_sort; ++i) {
            if (strlen(strings_array[i]) <= j) continue;
            if (strings_array[i][j] != first) break;
        }
        if (i < number_of_rows_to_sort) {
            radix_num = j;
            break;
        }
    }

    if (radix_num == -1)
        return;

    unsigned symbols[ASCII_SIZE] = {0};
    unsigned lines_via_symbol[ASCII_SIZE][number_of_rows_to_sort];

    for (array_size_t i = 0; i < number_of_rows_to_sort; ++i) {
        lines_via_symbol[(unsigned)strings_array[i][radix_num]][symbols[(unsigned)strings_array[i][radix_num]]] = i;
        symbols[(unsigned)strings_array[i][radix_num]]++;
    }

    array_size_t count = 0;
    strings_array_t line_array_copy = (strings_array_t) malloc(number_of_rows_to_sort * sizeof(char *));

    if (cmp("a", "b") < 0) {
        for (int i = 0; i < ASCII_SIZE; ++i) {
            if (symbols[i] > 0) {
                strings_array_t buf = malloc(symbols[i] * sizeof(char *));
                for (array_size_t j = 0; j < symbols[i]; j++) {
                    buf[j] = strings_array[lines_via_symbol[i][j]];
                }
                if (symbols[i] > 1) radix(buf, symbols[i], cmp);
                for (array_size_t j = 0; j < symbols[i]; j++) {
                    line_array_copy[count] = buf[j];
                    count++;
                }
                free(buf);
            }
        }
    } else {
        for (int i = ASCII_SIZE - 1; i >= 0; i--) {
            if (symbols[i] > 0) {
                strings_array_t buf = malloc(symbols[i] * sizeof(char *));
                for (array_size_t j = 0; j < symbols[i]; j++) {
                    buf[j] = strings_array[lines_via_symbol[i][j]];
                }
                if (symbols[i] > 1) radix(buf, symbols[i], cmp);
                for (array_size_t j = 0; j < symbols[i]; j++) {
                    line_array_copy[count] = buf[j];
                    count++;
                }
                free(buf);
            }
        }
    }
    memcpy(strings_array, line_array_copy, number_of_rows_to_sort * sizeof(char *));
    
    free(line_array_copy);
}
