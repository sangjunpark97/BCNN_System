#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "common.h"
#include "core.h"

/* ================== Control Schemes1 ================== */
/* ALL SKIP '0' ==> Scheme 1 (Non-Skip) */
/* BOUNDARY_OP_SKIP & POOLING_WINDOW_OP_SKIP_IF_RESULT_BIT1 '1' ==> Scheme 2 (1-Skip) */
/* Scheme2 + POOLING_WINDOW_OP_SKIP_IF_RESULT_BIT0 '1' ==> Scheme 3 (1,0-Skip) */
/* Scheme3 + LAMBDA_PREDICTED_SCHEME ==> Scheme 4 (1,0-Skip, lambda-predicted) */
#define BOUNDARY_OP_SKIP 1
#define POOLING_WINDOW_OP_SKIP_IF_RESULT_BIT1 1
#define POOLING_WINDOW_OP_SKIP_IF_RESULT_BIT0 1
#define LAMBDA_PREDICTED_SCHEME 0

/* ================== Control Schemes2 (locality) ================== */

#define POOLING_WINDOW_LOCALITY_BIT1 1
#define POOLING_WINDOW_LOCALITY_BIT0 1
#define POOLING_WINDOW_EARLY_PREDICTED_BIT0 0

#define TOTAL_TEST_IMGS 1000
/* ===================================================== */

int boundary_op_skip = BOUNDARY_OP_SKIP;
int pooling_window_op_skip_if_result_BIT1 = POOLING_WINDOW_OP_SKIP_IF_RESULT_BIT1;
int pooling_window_op_skip_if_result_BIT0 = POOLING_WINDOW_OP_SKIP_IF_RESULT_BIT0;
int lambda_predicted_scheme = LAMBDA_PREDICTED_SCHEME;
int pooling_window_locality = POOLING_WINDOW_LOCALITY;
int pooling_window_early_predicted_BIT0 = POOLING_WINDOW_EARLY_PREDICTED_BIT0;
unsigned long long  cycle_count_1st_block = 0;
unsigned long long  cycle_count_2nd_block = 0;
unsigned long long  cycle_count_3rd_block = 0;
unsigned long long  cycle_count_4th_block = 0;
unsigned long long  cycle_count_5th_block = 0;
unsigned long long  cycle_count_6th_block = 0;
unsigned long long  cycle_count_7th_block = 0;
unsigned long long  cycle_count_8th_block = 0;
unsigned long long  cycle_count_9th_block = 0;

//#define INPUT_FMAP_FILE_DIR     "./dump/SVHN_Image_10000.bin"

#define INPUT_FMAP_FILE_DIR     "./dump/testset.bin"
#define WEIGHT_FILE_DIR         "./dump/F_hat_block_all.bin"
#define THRESHOLD_FILE_DIR      "./dump/T_block_all.bin"
#define BIAS_FILE_DIR           "./dump/bn_bias_factor.bin"  
#define SCALE_FILE_DIR          "./dump/bn_scale_factor.bin"
#define LABLE_FILE_DIR          "./dump/lable.bin"

int main(void) {

    block_param_t param[9];
    int max_num = 0;
    int correct_image = 0;

    param[0] = (block_param_t) {.w=32, .h=32, .c=128,  .w__=3, .h__=3, .c_=128,  .s=1, .w_=32, .h_=32, .type=FIRST_BLOCK, .block_num=1};
    param[1] = (block_param_t) {.w=32, .h=32, .c=128,  .w__=3, .h__=3, .c_=128,  .s=2, .w_=16, .h_=16, .type=HIDDEN_BLOCK, .block_num=2};
    param[2] = (block_param_t) {.w=16, .h=16, .c=128,  .w__=3, .h__=3, .c_=256,  .s=1, .w_=16, .h_=16, .type=HIDDEN_BLOCK, .block_num=3};
    param[3] = (block_param_t) {.w=16, .h=16, .c=256,  .w__=3, .h__=3, .c_=256,  .s=2, .w_=8,  .h_=8,  .type=HIDDEN_BLOCK, .block_num=4};
    param[4] = (block_param_t) {.w=8,  .h=8,  .c=256,  .w__=3, .h__=3, .c_=512,  .s=1, .w_=8,  .h_=8,  .type=HIDDEN_BLOCK, .block_num=5};
    param[5] = (block_param_t) {.w=8,  .h=8,  .c=512,  .w__=3, .h__=3, .c_=512,  .s=2, .w_=4,  .h_=4,  .type=HIDDEN_BLOCK, .block_num=6};
    param[6] = (block_param_t) {.w=1,  .h=1,  .c=8192, .w__=1, .h__=1, .c_=1024, .s=1, .w_=1,  .h_=1,  .type=HIDDEN_BLOCK, .block_num=7};
    param[7] = (block_param_t) {.w=1,  .h=1,  .c=1024, .w__=1, .h__=1, .c_=1024, .s=1, .w_=1,  .h_=1,  .type=HIDDEN_BLOCK, .block_num=8};
    param[8] = (block_param_t) {.w=1,  .h=1,  .c=1024, .w__=1, .h__=1, .c_=10,   .s=1, .w_=1,  .h_=1,  .type=LAST_BLOCK, .block_num=9}; 

    /*param[0] = (block_param_t) {.w=32, .h=32, .c=128,  .w__=5, .h__=5, .c_=128,  .s=2, .w_=16, .h_=16, .type=FIRST_BLOCK, .block_num=1};
    param[1] = (block_param_t) {.w=16, .h=16, .c=128,  .w__=3, .h__=3, .c_=128,  .s=1, .w_=16, .h_=16, .type=HIDDEN_BLOCK, .block_num=2};
    param[2] = (block_param_t) {.w=16, .h=16, .c=128,  .w__=3, .h__=3, .c_=256,  .s=1, .w_=16, .h_=16, .type=HIDDEN_BLOCK, .block_num=3};
    param[3] = (block_param_t) {.w=16, .h=16, .c=256,  .w__=5, .h__=5, .c_=256,  .s=4, .w_=4,  .h_=4,  .type=HIDDEN_BLOCK, .block_num=4};
    param[4] = (block_param_t) {.w=4,  .h=4,  .c=256,  .w__=3, .h__=3, .c_=512,  .s=1, .w_=4,  .h_=4,  .type=HIDDEN_BLOCK, .block_num=5};
    param[5] = (block_param_t) {.w=4,  .h=4,  .c=512,  .w__=3, .h__=3, .c_=512,  .s=1, .w_=4,  .h_=4,  .type=HIDDEN_BLOCK, .block_num=6};
    param[6] = (block_param_t) {.w=1,  .h=1,  .c=8192, .w__=1, .h__=1, .c_=1024, .s=1, .w_=1,  .h_=1,  .type=HIDDEN_BLOCK, .block_num=7};
    param[7] = (block_param_t) {.w=1,  .h=1,  .c=1024, .w__=1, .h__=1, .c_=1024, .s=1, .w_=1,  .h_=1,  .type=HIDDEN_BLOCK, .block_num=8};
    param[8] = (block_param_t) {.w=1,  .h=1,  .c=1024, .w__=1, .h__=1, .c_=10,   .s=1, .w_=1,  .h_=1,  .type=LAST_BLOCK, .block_num=9}; */

    /*param[0] = (block_param_t) {.w=32, .h=32, .c=128,  .w__=5, .h__=5, .c_=128,  .s=2, .w_=16, .h_=16, .type=FIRST_BLOCK, .block_num=1};
    param[1] = (block_param_t) {.w=16, .h=16, .c=128,  .w__=3, .h__=3, .c_=256,  .s=1, .w_=16, .h_=16, .type=HIDDEN_BLOCK, .block_num=2};
    param[2] = (block_param_t) {.w=16, .h=16, .c=256,  .w__=3, .h__=3, .c_=256,  .s=1, .w_=16, .h_=16, .type=HIDDEN_BLOCK, .block_num=3};
    param[3] = (block_param_t) {.w=16, .h=16, .c=256,  .w__=5, .h__=5, .c_=256,  .s=4, .w_=4,  .h_=4,  .type=HIDDEN_BLOCK, .block_num=4};
    param[4] = (block_param_t) {.w=4,  .h=4,  .c=256,  .w__=3, .h__=3, .c_=128,  .s=1, .w_=4,  .h_=4,  .type=HIDDEN_BLOCK, .block_num=5};
    param[5] = (block_param_t) {.w=4,  .h=4,  .c=128,  .w__=3, .h__=3, .c_=128,  .s=1, .w_=4,  .h_=4,  .type=HIDDEN_BLOCK, .block_num=6};
    param[6] = (block_param_t) {.w=1,  .h=1,  .c=2048, .w__=1, .h__=1, .c_=128, .s=1, .w_=1,  .h_=1,  .type=HIDDEN_BLOCK, .block_num=7};
    param[7] = (block_param_t) {.w=1,  .h=1,  .c=128, .w__=1, .h__=1, .c_=128, .s=1, .w_=1,  .h_=1,  .type=HIDDEN_BLOCK, .block_num=8};
    param[8] = (block_param_t) {.w=1,  .h=1,  .c=128, .w__=1, .h__=1, .c_=10,   .s=1, .w_=1,  .h_=1,  .type=LAST_BLOCK, .block_num=9};*/

    /*bitvector_t*  I_hat_data    = (bitvector_t *) malloc(16384*TOTAL_TEST_IMGS);
    bitvector_t*  F_hat_data    = (bitvector_t *) malloc(1770752);
    bitvector_t*  T_data        = (bitvector_t *) malloc(7680);
    float*        bias_data     = (float *) malloc(40);
    float*        scale_data    = (float *) malloc(40);
    int*          lable_data    = (int *)   malloc(8*TOTAL_TEST_IMGS);*/

    bitvector_t*  I_hat_data    = (bitvector_t *) malloc(16384*TOTAL_TEST_IMGS);
    bitvector_t*  F_hat_data    = (bitvector_t *) malloc(1770752);
    bitvector_t*  T_data        = (bitvector_t *) malloc(7680);
    float*        bias_data     = (float *) malloc(40);
    float*        scale_data    = (float *) malloc(40);
    int*          lable_data    = (int *)   malloc(8*TOTAL_TEST_IMGS);
    load_param(INPUT_FMAP_FILE_DIR,     16384*TOTAL_TEST_IMGS,    I_hat_data);
    load_param(WEIGHT_FILE_DIR,         1770752,                   F_hat_data);
    load_param(THRESHOLD_FILE_DIR,      7680,                     T_data);
    load_param(BIAS_FILE_DIR,           40,                       bias_data);
    load_param(SCALE_FILE_DIR,          40,                       scale_data);
    load_param(LABLE_FILE_DIR,          8*TOTAL_TEST_IMGS,        lable_data);

    for(int i = 0 ; i < TOTAL_TEST_IMGS; i ++){
        max_num = block_proc_sw(I_hat_data, F_hat_data, T_data, param, 9, bias_data, scale_data); 
        if(max_num == lable_data[2*i]) correct_image += 1;
        I_hat_data += 1024;

        if(i%100==0){
            printf("[%d]th image processed.. accuracy : %f \n", i, (float) correct_image / (float) i);
        } 
    }

    float accuarcy = (float) correct_image / (float) TOTAL_TEST_IMGS;

    printf("\n");
    printf("TOTAL_TEST_IMGS = %d \n",   TOTAL_TEST_IMGS);
    printf("correct_image = %d \n", correct_image);
    printf("Final accuracy = %f \n\n", accuarcy);

    printf("========== cycle count 1 - 9th block ==========\n");
    printf("%lld \n", cycle_count_1st_block / TOTAL_TEST_IMGS);
    printf("%lld \n", cycle_count_2nd_block / TOTAL_TEST_IMGS);
    printf("%lld \n", cycle_count_3rd_block / TOTAL_TEST_IMGS);
    printf("%lld \n", cycle_count_4th_block / TOTAL_TEST_IMGS);
    printf("%lld \n", cycle_count_5th_block / TOTAL_TEST_IMGS);
    printf("%lld \n", cycle_count_6th_block / TOTAL_TEST_IMGS);
    printf("%lld \n", cycle_count_7th_block / TOTAL_TEST_IMGS);
    printf("%lld \n", cycle_count_8th_block / TOTAL_TEST_IMGS);
    printf("%lld \n", cycle_count_9th_block / TOTAL_TEST_IMGS);

    return 0;
}
