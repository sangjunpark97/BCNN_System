#include "core.h"
#include "common.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

extern int boundary_op_skip;
extern int pooling_window_op_skip_if_result_BIT1;
extern int pooling_window_op_skip_if_result_BIT0;
extern int lambda_predicted_scheme;
extern int pooling_window_locality_BIT1;
extern int pooling_window_locality_BIT0;
extern int pooling_window_early_predicted_BIT0;

extern unsigned long long  cycle_count_1st_block;
extern unsigned long long  cycle_count_2nd_block;
extern unsigned long long  cycle_count_3rd_block;
extern unsigned long long  cycle_count_4th_block;
extern unsigned long long  cycle_count_5th_block;
extern unsigned long long  cycle_count_6th_block;
extern unsigned long long  cycle_count_7th_block;
extern unsigned long long  cycle_count_8th_block;
extern unsigned long long  cycle_count_9th_block;

// local : Detected location of BIT1 in previous pooling window
s32 local[512];
// TF : previous pooling window BIT Result
s32 TF[512];

u16 lambda_bias(s32 delta, s32 gamma, block_param_t* param) {
    switch (param->type) {
        case FIRST_BLOCK:
            return 255 * 3 * ((param->w__ * param->h__) - ((MIN(param->h - delta - 1, param->h__ / 2) + MIN(delta, param->h__ / 2)) + 1) * ((MIN(param->w - gamma - 1, param->w__ / 2) + MIN(gamma, param->h__ / 2)) + 1));
        default:
            return param->c * ((param->w__ * param->h__) - ((MIN(param->h - delta - 1, param->h__ / 2) + MIN(delta, param->h__ / 2)) + 1) * ((MIN(param->w - gamma - 1, param->w__ / 2) + MIN(gamma, param->h__ / 2)) + 1));
    }
}
s32 c_local;
void block_proc(bitvector_t* I_hat, bitvector_t* F_hat, bitvector_t* O_hat, threshold_t* T, block_param_t* param) {
    long long cycle_count = 0;
    bitvector_t temp_result;
    u16 lambda = 0;
    u32 x,m,n;
    s32 BIT0_count;
    float RELAX_FACTOR=1;
    s32 sum;
    for (s32 y = 0; y < param->h_; y++) {
        for (s32 x_ = 0; x_ < param->w_; x_++) {
            if((y%2==1)) x=(param->w_-1)-x_;
            else                                  x=x_;
            if((x+y)==0){
                for(s32 F=0;F<512;F++){
                    TF[F]=1;
                }
            }
            s32 d = 0;
            while (d < param->c_) {
                bit_t result_bit = BIT0;
                BIT0_count=0;
                for (s32 n_ = 0; n_ < param->s; n_++) {
                    for (s32 m_ = 0; m_ < param->s; m_++) {
                        if(pooling_window_early_predicted_BIT0&&(param->s==4)&&(BIT0_count>=14)&&(TF[d]==BIT0)){
                                goto end_pooling_window_if_result_BIT0;
                            }
                        else if(pooling_window_early_predicted_BIT0&&(param->s==2)&&(BIT0_count>=3)&&(TF[d]==BIT0)){
                                goto end_pooling_window_if_result_BIT0;
                            }
                        if(param->s==2&&TF[d]&&pooling_window_locality_BIT1){
                            if(y%2==0){
                                if(local[d]<2){
                                    m=n_;
                                    n=m_;
                                }
                                else{
                                    m=n_;
                                    n=1-m_;
                                }
                            }
                            else{
                                if(local[d]<2){
                                    m=1-n_;
                                    n=m_;
                                }
                                else{
                                    m=1-n_;
                                    n=1-m_;
                                }
                            }
                        }
                        else if(param->s==2&&!TF[d]&&pooling_window_locality_BIT0){
                            if(y%2==0){
                                m=1-n_;
                                n=m_;
                            }
                            else{
                                m=n_;
                                n=m_;
                            }
                        }
                        else{
                            m=m_;
                            n=n_;
                        }
                        s32 gamma = param->s * x + m;
                        s32 delta = param->s * y + n;
                        s32 N;
                        if (param->type == FIRST_BLOCK) N = (((param->w__ * param->h__ * 255 * 3) - lambda_bias(delta, gamma, param)) / (255 * 3)) * 3;
                        else                            N = ((param->w__ * param->h__ * param->c) - lambda_bias(delta, gamma, param)) / ALPHA;
                        lambda = lambda_bias(delta, gamma, param);
                        if(ALPHA==8) cycle_count += boundary_op_skip ? 0 : (param->block_num == 1) ? (lambda / (255 * 3)) * 3 : lambda / ALPHA;
                        else cycle_count += boundary_op_skip ? 0 : (param->block_num == 1) ? (lambda / (255 * 3)) * (128/ALPHA) : lambda / ALPHA;
                        s32 i = 0;
                        s32 N_count = 0;
                        for (s32 v = -MIN(delta, (param->h__ / 2)); v <= MIN(param->h - delta - 1, (param->h__ / 2)); v++) {
                            for (s32 u = -MIN(gamma, (param->w__ / 2)); u <= MIN(param->w - gamma - 1, (param->w__ / 2)); u++) {
                                for (s32 t = 0; t < (param->c / 128); t++) {
                                    u32 iter_start = 0;
                                    u32 iter_end = ALPHA/8; 
                                    for (s32 l = 0; l < 128/ALPHA; l++) {
                                        cycle_count += 1;
                                        N_count += 1;
                                        switch (param->type) {
                                            case FIRST_BLOCK:
                                                if((ALPHA==8) && (l>=3)) {
                                                    cycle_count -= 1;
                                                    N_count -= 1;
                                                    goto SKIP;
                                                }
                                                lambda += 2 * binary_weighted_sum(
                                                    &I_hat[((delta + v) * param->w + gamma + u) * (param->c / 128) + t],
                                                    &F_hat[((d * param->h__ + v + (param->h__ / 2)) * param->w__ + u + (param->w__ / 2)) * (param->c / 128) + t],
                                                    iter_start, iter_end);
                                                break;
                                            default:
                                                lambda += 2 * xnor_popcount(
                                                    &I_hat[((delta + v) * param->w + gamma + u) * (param->c / 128) + t],
                                                    &F_hat[((d * param->h__ + v + (param->h__ / 2)) * param->w__ + u + (param->w__ / 2)) * (param->c / 128) + t],
                                                    iter_start, iter_end);
                                                break;
                                        }
                                        i++;
                                        SKIP:
                                        if (lambda >= RELAX_FACTOR*T[d]) result_bit = BIT1;
                                        switch (param->type) {
                                            case FIRST_BLOCK:
                                                if (pooling_window_op_skip_if_result_BIT0 && ((N-i)!=0) && ((lambda + ((N - i) *2* 255 * 3))< T[d])){
                                                    result_bit = BIT0;
                                                    //printf("%d ",N-i);
                                                    goto end_pooling_window_if_result_BIT0;
                                                }
                                                break;
                                            default:
                                                if (pooling_window_op_skip_if_result_BIT0 && ((N-i)!=0) && ((lambda + ((N - i) *2* ALPHA)) < T[d]) && !(param->type == LAST_BLOCK)){
                                                    result_bit = BIT0;
                                                    if(param->block_num==5) {
                                                        sum+=N-i;
                                                        //printf("%d ",sum);
                                                    }
                                                    goto end_pooling_window_if_result_BIT0;
                                                }
                                                break;
                                        }
                                        if (pooling_window_op_skip_if_result_BIT1 && (result_bit == BIT1) && !(param->type == LAST_BLOCK)){
                                            if(param->s!=1)local[d]=n*2+m;
                                            TF[d]=1;
                                            goto end_pooling_window_if_result_BIT1;
                                        }
                                    
                                        iter_start += ALPHA/8; 
                                        iter_end   += ALPHA/8; 
                                    }
                                }
                            }
                        }
                    end_pooling_window_if_result_BIT0: 
                    BIT0_count++;
                    }
                }
            end_pooling_window_if_result_BIT1:
            if(!result_bit){
                TF[d]=0;
            }
                switch (param->type) {
                    case LAST_BLOCK:
                        temp_result.byte0 = lambda & 0xFF;
                        temp_result.byte1 = (lambda >> 8) & 0xFF;
                        O_hat[d] = temp_result;
                        break;
                    default:
                        left_shift_by_1bit(&temp_result, result_bit);
                        if (d % 128 == (128 - 1)) O_hat[((param->w_ * y + x) * param->c_ + d) / 128] = temp_result;
                        break;
                }
                d++;
            }
        }
    }
    if(param->block_num == 1)       cycle_count_1st_block += cycle_count;
    else if(param->block_num == 2)  cycle_count_2nd_block += cycle_count;
    else if(param->block_num == 3)  cycle_count_3rd_block += cycle_count;
    else if(param->block_num == 4)  cycle_count_4th_block += cycle_count;
    else if(param->block_num == 5)  cycle_count_5th_block += cycle_count;
    else if(param->block_num == 6)  cycle_count_6th_block += cycle_count;
    else if(param->block_num == 7)  cycle_count_7th_block += cycle_count;
    else if(param->block_num == 8)  cycle_count_8th_block += cycle_count;
    else if(param->block_num == 9)  cycle_count_9th_block += cycle_count;
    
}

int block_proc_sw(bitvector_t* I_hat_data, bitvector_t* F_hat_data, bitvector_t* T_data, block_param_t* param, int block_num, float* bias_data, float* scale_data) {
    bitvector_t* I_hat = 0;
    bitvector_t* F_hat = 0;
    bitvector_t* O_hat = 0;
    bitvector_t* temp_O_hat = 0;
    threshold_t* T = 0;

    for (int i = 0; i < 9; i++) {
        I_hat = (bitvector_t*)malloc(param[i].w * param[i].h * param[i].c / 8);
        F_hat = (bitvector_t*)malloc(param[i].w__ * param[i].h__ * param[i].c * param[i].c_ / 8);
        if (param[i].type == LAST_BLOCK)    O_hat = (bitvector_t*)malloc(param[i].w_ * param[i].h_ * param[i].c_ * 16);
        else                                O_hat = (bitvector_t*)malloc(param[i].w_ * param[i].h_ * param[i].c_ / 8);
        T = (threshold_t*)malloc(param[i].c_ * 16);

        if (param[i].type == FIRST_BLOCK)   memcpy(I_hat, I_hat_data, 16384);

        memcpy(F_hat, F_hat_data, param[i].c_ * param[i].w__ * param[i].h__ * param[i].c / 8);
        memcpy(T, T_data, param[i].c_ * 2);

        F_hat_data += param[i].w__ * param[i].h__ * param[i].c * param[i].c_ / 128;
        T_data += param[i].c_ * 16 / 128;

        if (param[i].type == FIRST_BLOCK)   block_proc(I_hat, F_hat, O_hat, T, &param[i]);
        else                                block_proc(temp_O_hat, F_hat, O_hat, T, &param[i]);

        if (param[i].type != FIRST_BLOCK)   free(temp_O_hat);

        temp_O_hat = O_hat;

        free(I_hat);
        free(F_hat);
        free(T);
    }
    char* result_softmax = (char*)malloc(40);
    for (int i = 0; i < 10; i++) {
        result_softmax[4 * i + 0] = O_hat[i].byte0;
        result_softmax[4 * i + 1] = O_hat[i].byte1;
        result_softmax[4 * i + 2] = O_hat[i].byte2;
        result_softmax[4 * i + 3] = O_hat[i].byte3;
    }
    int* output_result;
    output_result = (int*)result_softmax;

    float* scale_bias_result = (float*)malloc(40);
    float* final_data = (float*)malloc(40);

    mul_scale(scale_bias_result, output_result, scale_data, bias_data);
    softmax(final_data, scale_bias_result, 10);

    float max_num = 0;
    int max_location = 0;

    max_class(final_data, &max_num, &max_location, 10);

    free(result_softmax);
    free(scale_bias_result);
    free(final_data);
    free(temp_O_hat);

    return max_location;
}