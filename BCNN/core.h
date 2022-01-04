#pragma once
#include "common.h"

#define HIDDEN_BLOCK 0
#define FIRST_BLOCK 1
#define LAST_BLOCK 2

typedef struct {
    u16 w;     
    u16 h;      
    u16 c;   
    u16 w__;    
    u16 h__;    
    u16 c_;  
    u16 s;      
    u16 w_;  
    u16 h_;
    u16 block_num;
    enum {
        hidden = HIDDEN_BLOCK,
        first = FIRST_BLOCK,
        last = LAST_BLOCK
    } type;
} block_param_t;

void block_proc(bitvector_t* I_hat, bitvector_t* F_hat, bitvector_t* O_hat, threshold_t* T, block_param_t* param);
int block_proc_sw(bitvector_t* I_hat_data, bitvector_t* F_hat_data, bitvector_t* T_data, block_param_t *param, int block_num, float* bias_data, float* scale_data);
