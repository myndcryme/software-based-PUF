// ------------------------------------------------------------------------
//
//        File: Tools.h
//        Date: April 15, 2018
//        Created by: Ade Setyawan Sajim as part of his MSc Thesis project
//
// This code contains some generic utilities code that required in the
// development of the thesis project
//

#include "Tools.h"

Tools::Tools() {}


int8_t Tools::xor_int(int8_t a, int8_t b) {
    return a == b ? 0 : 1;
}

void Tools::convert_uint8_to_bin_array(uint8_t input, int8_t *result) {
    for (int8_t i = 0; i < 8; i++) {
        if ((input >> (7 - i) & 0x1) == 1) {
            result[i] = 1;
        }
    }
}

void Tools::convert_bin_array_to_uint8_array(int8_t *binary, uint8_t *result, int length) {
    int index = 0;
    int ro = length % 8 ? length + 8 - (length % 8) : length;
    for (int i = 0; i < ro; i++) {
        if (i < length) {
            if (binary[i] == 1) {
                result[index] = result[index] | 0x1;
            }
        }
        if ((i + 1) % 8 == 0) {
            index++;
        } else {
            result[index] = result[index] << 1;
        }
    }
}

void Tools::convert_uint8_array_to_bin_array(uint8_t *input, int8_t *result, int length) {
    for (int8_t j = 0; j < length; j++) {
        convert_uint8_to_bin_array(input[j], &result[j * 8]);
    }
}

void Tools::combine_two_bin_array_to_uint8_array(int8_t *bin1, int length_bin1, int8_t *bin2, int length_bin2,
                                                 uint8_t *result) {
    int8_t temp[length_bin1 + length_bin2];
    memcpy(temp, bin1, length_bin1);
    memcpy(&temp[length_bin1], bin2, length_bin2);
    convert_bin_array_to_uint8_array(temp, result, length_bin1 + length_bin2);
}

uint8_t Tools::ceil(uint8_t i, uint8_t div) {
    return static_cast<uint8_t>(i % div ? i / div + 1 : i / div);
}

void Tools::xor_binary_array(uint8_t *a, uint8_t *b, uint8_t *result, int size) {
    for (int i = 0; i < ceil(size, 8); i++) {
        result[i] = a[i] ^ b[i];
    }
}

void Tools::gen_key_per_row(uint8_t *keys, uint8_t *result) {
    int index_result = 0, index_key = 0;
    uint8_t shift;
    for (int i = 0; i < 256; i++) {
        shift = 7 - (i % 8);
        if ((keys[index_key] >> shift) & 0x1) {
            result[index_result] = result[index_result] | 0x1;
        }
        if ((i + 1) % 7 == 0) {
            result[index_result] = result[index_result] << 1;
            index_result++;
        } else {
            result[index_result] = result[index_result] << 1;
        }
        if ((i + 1) % 8 == 0) {
            index_key++;
        }
    }
    result[index_result] = result[index_result] << 3;
}

void Tools::convert_key_back(uint8_t *key_37, uint8_t *key_32){
    int a = 0;
    int b = 0;
    for (int j=0;j<256;j++) {
        key_32[a] = static_cast<uint8_t>(key_32[a] | (key_37[b] >> (7 - (j % 7)) & 0x1));
        if ((j + 1) % 8 == 0) {
            a++;
        }else{
            key_32[a] = key_32[a] << 1;
        }
        if ((j + 1) % 7 == 0) {
            b++;
        }
    }
}
