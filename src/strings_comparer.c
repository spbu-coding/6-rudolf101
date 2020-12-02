#include <stdio.h>
#include <stdlib.h>
#include "sortings.h"

#define REQUIRED_NUMBER_OF_ARGUMENTS 6
#define DECIMAL_NOTATION 10
#define NUMBER_OF_SORTS 5
#define NUMBER_OF_COMPARATORS 2

#define error(...) (fprintf(stderr, __VA_ARGS__))

typedef void (*sort_func_t)(strings_array_t, array_size_t, comparator_func_t);

int quick_asc_comparator(const char *str1, const char *str2){
    return strcmp(*(char**)str1, *(char**)str2);
}

int quick_des_comparator(const char *str1, const char *str2){
    return -strcmp(*(char**)str1, *(char**)str2);
}

int asc_comparator(const char *str1, const char *str2) {
    return strcmp(str1, str2);
}

int des_comparator(const char *str1, const char *str2) {
    return -strcmp(str1, str2);
}

int read_from_file(strings_array_t strings_array, array_size_t number_of_rows_to_sort, char *input_file_name){
    FILE *input_file;
    if ((input_file = fopen(input_file_name, "rb")) == NULL) {
        error("Unable to open input file, check the filename.\n");
        return -1;
    }

    for(array_size_t i = 0; i < number_of_rows_to_sort; ++i){
        if(fgets(strings_array[i], MAX_INPUT_STRING_SIZE, input_file) == NULL){
            error("Unable to read %zu line in the file.\n", i + 1);
            return -1;
        }
        // We check whether there is a transition to a new line at the end of each line, if not, we add it.
        if(strchr(strings_array[i], '\n') == NULL)
            strcat(strings_array[i], "\n");
    }

    fclose(input_file);
    return 0;
}

int print_to_file(strings_array_t strings_array, array_size_t number_of_rows_to_sort, char *output_file_name){
    FILE *output_file;
    if ((output_file = fopen(output_file_name, "wb")) == NULL) {
        error("Unable to open output file, check the permission to create files in the folder.\n");
        return -1;
    }
    for(array_size_t i = 0; i < number_of_rows_to_sort; ++i){
        if(fputs(strings_array[i], output_file) == EOF){
            error("Unable to write %zu line to the file\n", i + 1);
            return -1;
        }
    }
    fclose(output_file);
    return 0;
}

void free_array(strings_array_t strings_array, array_size_t size){
    for(array_size_t i = 0; i < size; ++i)
        free(strings_array[i]);
    free(strings_array);
}

int main(int argc, char *argv[]) {

    if (argc != REQUIRED_NUMBER_OF_ARGUMENTS) {
        error("Invalid number of arguments, check the correct input using the following form:\n"
              "<number of rows> <input_file>.txt <output_file>.txt <name of the sorting algorithm> <name of the comparator>\n");
        return -1;
    }

    char *EndPtr = argv[1];
    array_size_t number_of_rows_to_sort = strtol(argv[1], &EndPtr, DECIMAL_NOTATION);
    if (strlen(EndPtr)) {
        error("The number of rows must be a number, invalid data was found: %s\n", EndPtr);
        return -1;
    }

    char *input_file_name = argv[2];
    char *output_file_name = argv[3];

    char *name_of_sorting_functions[NUMBER_OF_SORTS] = {"bubble", "insertion", "merge", "quick", "radix"};
    sort_func_t sorting_functions[NUMBER_OF_SORTS] = {&bubble, &insertion, &merge, &quick, &radix};
    sort_func_t sorting_method = NULL;
    for(unsigned int i = 0; i < NUMBER_OF_SORTS; ++i)
        if(strcmp(name_of_sorting_functions[i], argv[4]) == 0)
            sorting_method = sorting_functions[i];
    if(sorting_method == NULL){
        error("Unknown sorting method %s, you can use only bubble, insertion, merge, quick or radix.\n", argv[4]);
        return -1;
    }

    char *name_of_comparators[NUMBER_OF_COMPARATORS] = {"asc", "des"};
    comparator_func_t comparator_functions[NUMBER_OF_COMPARATORS] = {&asc_comparator, &des_comparator};
    comparator_func_t comparing_method = NULL;
    for(unsigned int i = 0; i < NUMBER_OF_COMPARATORS; ++i)
        if(strcmp(name_of_comparators[i], argv[5]) == 0)
            comparing_method = comparator_functions[i];
    if(comparing_method == NULL){
        error("Unknown comparing method %s, you can use only asc or des.\n", argv[5]);
        return -1;
    }
    // Since I use the built-in quick sorting function, I had to change the comparator
    if(sorting_method == &quick) {
        if (comparing_method == &asc_comparator)
            comparing_method = &quick_asc_comparator;
        else
            comparing_method = &quick_des_comparator;
    }

    strings_array_t strings_array = (char **) malloc(sizeof(char*) * number_of_rows_to_sort);
    if(strings_array == NULL){
        error("Cannot allocate memory for strings array.\n");
        return -1;
    }
    for(array_size_t i = 0; i < number_of_rows_to_sort; ++i){
        strings_array[i] = (char *) malloc(sizeof(char) * MAX_INPUT_STRING_SIZE);
        if(strings_array[i] == NULL){
            free_array(strings_array, i);
            error("Cannot allocate memory for %zu string\n", i + 1);
            return -1;
        }
    }

    int return_value = read_from_file(strings_array, number_of_rows_to_sort, input_file_name);
    if(return_value)
        return return_value;

    sorting_method(strings_array, number_of_rows_to_sort, comparing_method);

    return_value = print_to_file(strings_array, number_of_rows_to_sort, output_file_name);
    if(return_value)
        return return_value;

    free_array(strings_array, number_of_rows_to_sort);
    return 0;
}
