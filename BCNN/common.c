#include "common.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

static u16 countone(unsigned a) {
	u16 result = 0;
	for (s32 i = 0; i < 8; i++)
		result += BIT(a, i);
	return result;
}

u16 binary_weighted_sum(bitvector_t* a, bitvector_t* b, u32 iter_start, u32 iter_end) {
	u16 result = 0;
	u8* tmp_a = (u8*) a;
	u8* tmp_b = (u8*) b;

	for(u32 i = iter_start; i < iter_end; i++){
		result += (u8) (~(tmp_a[i] ^ tmp_b[i]));
	}
	return result;
}

u16 xnor_popcount(bitvector_t* a, bitvector_t* b, u32 iter_start, u32 iter_end) {
	u8  tmp_result=0;
	u16 result=0;
	u8* tmp_a = (u8*) a;
	u8* tmp_b = (u8*) b;

	for(u32 i = iter_start; i < iter_end; i++){
		tmp_result = (u8) (~(tmp_a[i] ^ tmp_b[i]));
		result += countone(tmp_result);
	}
	return result;
}

bit_t left_shift_by_1bit(bitvector_t* src, bit_t bit) {
	bit_t escaping_bit = BIT(src->byte0, 7);
	bit_t msb_1  = BIT(src->byte1, 7);
	bit_t msb_2  = BIT(src->byte2, 7);
	bit_t msb_3  = BIT(src->byte3, 7);
	bit_t msb_4  = BIT(src->byte4, 7);
	bit_t msb_5  = BIT(src->byte5, 7);
	bit_t msb_6  = BIT(src->byte6, 7);
	bit_t msb_7  = BIT(src->byte7, 7);
	bit_t msb_8  = BIT(src->byte8, 7);
	bit_t msb_9  = BIT(src->byte9, 7);
	bit_t msb_10 = BIT(src->byte10, 7);
	bit_t msb_11 = BIT(src->byte11, 7);
	bit_t msb_12 = BIT(src->byte12, 7);
	bit_t msb_13 = BIT(src->byte13, 7);
	bit_t msb_14 = BIT(src->byte14, 7);
	bit_t msb_15 = BIT(src->byte15, 7);

	src->byte0  = LSH_INSERT(src->byte0, msb_1);
	src->byte1  = LSH_INSERT(src->byte1, msb_2);
	src->byte2  = LSH_INSERT(src->byte2, msb_3);
	src->byte3  = LSH_INSERT(src->byte3, msb_4);
	src->byte4  = LSH_INSERT(src->byte4, msb_5);
	src->byte5  = LSH_INSERT(src->byte5, msb_6);
	src->byte6  = LSH_INSERT(src->byte6, msb_7);
	src->byte7  = LSH_INSERT(src->byte7, msb_8);
	src->byte8  = LSH_INSERT(src->byte8, msb_9);
	src->byte9  = LSH_INSERT(src->byte9, msb_10);
	src->byte10 = LSH_INSERT(src->byte10, msb_11);
	src->byte11 = LSH_INSERT(src->byte11, msb_12);
	src->byte12 = LSH_INSERT(src->byte12, msb_13);
	src->byte13 = LSH_INSERT(src->byte13, msb_14);
	src->byte14 = LSH_INSERT(src->byte14, msb_15);
	src->byte15 = LSH_INSERT(src->byte15, bit);

	return escaping_bit;
}

void read_file(char* file_dir, void* dst, u32 sz) {
	FILE* opened_file = fopen(file_dir, "rb");
	if (opened_file == NULL) {
		printf("Error on file open");
	}
	fread(dst, sizeof(char), sz, opened_file);
	fclose(opened_file);
}

void write_file(char* file_dir, void* src, u32 sz) {
	FILE* opened_file = fopen(file_dir, "wb");
	if (opened_file == NULL) {
		printf("Error on file open");
	}
	else fwrite(src, sizeof(u32), sz, opened_file);
	fclose(opened_file);
}

void load_param(char *file_dir, int file_size, void *pointer){
    FILE *file;
    file = fopen(file_dir, "rb");
    if(pointer == NULL) printf("memory excess: threshold\n");
    fread(pointer, sizeof(char), file_size, file);
    fclose(file);

}

void softmax(float *output, float *input, int size){
	double sum = 0;
	int i;
	for (i=0; i<size; i++){
		sum += exp(input[i]);
	}
	for (i=0; i<size; i++) {
		output[i] = (exp(input[i]) / sum);
	}
}

void mul_scale(float *output, int *input, float *scale, float *bias){
	for (int i=0; i<10; i++){
		output[i] = input[i] * scale[i] + bias[i];
	}
}

void max_class(float* p, float* max, int* maxlocation, int num){
	int i;
	*max = *p;
	*maxlocation = 0;

	for(i = 0; i < num; i++){
		if(*max < p[i]){
			*max = p[i];
			*maxlocation = i;
		}
	}
}
