#pragma once

/* ============== Control parameters ============== */
// ALPHA = 8 for Small Pruning Unit
#define ALPHA 128
#define K 2
#define P 1.5
#define Q 2
/* ================================================ */

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define BIT(a, idx) (((a) >> (idx)) & 0x1)
#define LSH_INSERT(a, bit) (((a) << 1) | (bit))

typedef char s8;
typedef short s16;
typedef int s32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef u8 bit_t; //bit type

#define BIT0 (bit_t)0
#define BIT1 (bit_t)1

typedef struct {
    u8 byte0;
    u8 byte1;
    u8 byte2;
    u8 byte3;
    u8 byte4;
    u8 byte5;
    u8 byte6;
    u8 byte7;
    u8 byte8;
    u8 byte9;
    u8 byte10;
    u8 byte11;
    u8 byte12;
    u8 byte13;
    u8 byte14;
    u8 byte15;
} bitvector_t;

typedef u16 threshold_t;

u16 xnor_popcount(bitvector_t* a, bitvector_t* b, u32 iter_start, u32 iter_end);
u16 binary_weighted_sum(bitvector_t* a, bitvector_t* b, u32 iter_start, u32 iter_end);

bit_t left_shift_by_1bit(bitvector_t *src, bit_t bit);
void read_file(char* file_dir, void* dst, u32 sz);
void write_file(char* file_dir, void* src, u32 sz);
void load_param(char *file_dir, int file_size, void *pointer);
void softmax(float *output, float *input, int size);
void mul_scale(float *output, int *input, float *scale, float *bias);
void max_class(float* p, float* max, int* maxlocation, int num);