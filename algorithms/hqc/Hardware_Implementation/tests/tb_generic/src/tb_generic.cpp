/**
 * @file tb_generic.cpp
 * @brief Provides a generic testbench function.
 */

#include <stdio.h>
#include <stdlib.h>
#include <kem_typedef.h>

#define STR_MAXLEN 128

/**
 * @brief Checks that a given function gives an output equal to the contents of a file.
 *
 * Calls the function pointer f given as argument with the following parameters: an output byte array of size
 * compare_bytesize; and the compare_bytesize integer. Writes the output byte array in hex into a file ../data/test_%s.dat
 * with %s replaced with the parameter name given as input. Checks that this file is equal to ../data/data_%s.dat with
 * a system command and returns 0 if they are equal or 1 if not.
 *
 * @param[in] f function pointer of the function to be tested
 * @param[in] name string with the name needed to find the reference data file
 * @param[in] compare_bytesize number of bytes of the output byte array that need to be compared to the data file
 * @returns 0 if the check is a success and 1 if not
 */
int tb_generic_call(void (*f)(uint8_t *, size_t), const char *name, size_t compare_bytesize) {
    char filename[STR_MAXLEN]="";
    char diff_command[STR_MAXLEN]="";
    snprintf(filename, STR_MAXLEN, "../data/test_%s.dat", name);
    FILE* test_file = fopen(filename, "w");
    uint8_t output[compare_bytesize];
    int val_diff = 1;

    (*f)(output, compare_bytesize);
    for (int i=0; i<compare_bytesize; ++i) {
        fprintf(test_file, "%02X", (unsigned char)output[i]);
    }

    fclose(test_file);

    snprintf(diff_command, STR_MAXLEN, "diff --brief -w ../data/test_%s.dat ../data/data_%s.dat", name, name);

    val_diff = system(diff_command);

    if (val_diff != 0) {
        printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        printf("!!!           %s                   \n", name);
        printf("!!!           TESTS FAILED           !!!\n");
        printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        return 1;
    }

    printf("\n========================================\n");
    printf("===           %s                   \n", name);
    printf("===           Tests passed           ===\n");
    printf("========================================\n");
    return 0;
}
