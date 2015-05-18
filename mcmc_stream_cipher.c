#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <tgmath.h>
#include <time.h>
#include "stream_cipher.h"

#define MAX_FILE 1000000

int main(int argc, char** argv) {

    assert(argc == 4);
    char* file_test = (char*)malloc(30 * sizeof(char));
    strcpy(file_test, argv[1]);
    int niter = atoi(argv[2]);
    int cycle = atoi(argv[3]);

    FILE* fp = fopen(file_test, "r");
    char* text = (char*)malloc(MAX_FILE * sizeof(char));
    char* plain = (char*)malloc(MAX_FILE * sizeof(char));
    char* cipher = (char*)malloc(MAX_FILE * sizeof(char));
    char* recover = (char*)malloc(MAX_FILE * sizeof(char));
    int text_len = 0;
    while(fscanf(fp, "%c", &text[text_len++]) != EOF);
    fclose(fp);
    memcpy(plain, text, text_len * sizeof(char));

    /* Key */
    srand(time(NULL));
    key_t key;
    key = (char*)malloc(KEY_LEN * sizeof(char));
    key_init(key);
    
    /* Encrypt */
    // printf("Plain text:\n%s\n", text);
    // text_print(text, text_len);
    encrypt(text, cipher, key, text_len);
    // printf("Cipher text:\n");
    // text_print(text, text_len);

    /* train */
    score_t text_score = (double**)malloc(128 * sizeof(double*));
    score_t train_score = (double**)malloc(128 * sizeof(double*));
    for(int i = 0; i < 128; i++) {
        text_score[i] = (double*)malloc(128 * sizeof(double));
        train_score[i] = (double*)malloc(128 * sizeof(double));
    }
    // read train score in logarithm
    fp = fopen("ratio", "r");
    int train_len, tmp;
    double target;
    fscanf(fp, "%d", &train_len);
    for(int i = 0; i < 128; i++) {
        for(int j = 0; j < 128; j++) {
            fscanf(fp, "%d", &tmp);
            train_score[i][j] = tmp;
        }
    }
    fclose(fp);


    /* MCMC Start */
#ifdef DEBUG 
    printf("Start MCMC\n");
#endif
    srand(time(NULL));
    
    // key init
    key_t guess_key = (char*)malloc(KEY_LEN * sizeof(char));
    key_t max_key = (char*)malloc(KEY_LEN * sizeof(char));
    for(int i = 0; i < KEY_LEN; i++)
        guess_key[i] = 0;

    double target_tmp, target_max = 0, alpha;
    char* guess_text = (char*)malloc(MAX_FILE * sizeof(char));
    decrypt(guess_text, cipher, guess_key, text_len);
    score_update(guess_text, text_score, text_len);
    target = target_cal(text_score, train_score, text_len, train_len);
    
    int guess_pos, guess_num;
    int block_size = 1;
    int block = niter / (((KEY_LEN / block_size) + 1) * cycle);

    for(int c = 0; c < cycle; c++) {
        for(int b = 0; b <= KEY_LEN / block_size; b++) {
            for(int n = 0; n < block; n++) {
                // key shuffle
                if(b == KEY_LEN / block_size)
                    guess_pos = rand() % KEY_LEN;
                else
                    guess_pos = rand() % block_size + block_size * b;
                if(c > cycle * 0.4) {
                    if(c < 8)
                        guess_num = rand() % 64;
                    else if(c < cycle * 0.7)
                        guess_num = rand() % 16;
                    else
                        guess_num = rand() % 8;
                    if(rand() % 2 == 0)
                        guess_key[guess_pos] = ((int)guess_key[guess_pos] + guess_num) % 128;
                    else
                        guess_key[guess_pos] = ((int)guess_key[guess_pos] + 128 - guess_num) % 128;
                } else {
                    guess_num = rand() % 128;
                    guess_key[guess_pos] = ((int)guess_key[guess_pos] + guess_num) % 128;
                }
                decrypt(guess_text, cipher, guess_key, text_len);

                // update score & target
                score_update(guess_text, text_score, text_len);
                target_tmp = target_cal(text_score, train_score, text_len, train_len);
                
                // Metropolis-Hasting algorithm
                alpha = target_tmp / target;
                alpha = alpha * BETA;
                if(alpha < 1 && rand() > RAND_MAX * alpha){ // fail
                    guess_key[guess_pos] = ((int)guess_key[guess_pos] + (128 - guess_num)) % 128;
                }
                else { // change key
                    target = target_tmp;
                }
         
                // record max key
                if(target > target_max) {
                    target_max = target;
                    for(int i = 0; i < KEY_LEN; i++)
                        max_key[i] = guess_key[i];
                }
            }
            // update max key
            for(int i = 0; i < KEY_LEN; i++)
                guess_key[i] = max_key[i];
        }
#ifdef DEBUG
        printf("Cycle: %d Target: %lf\n", c+1, target_max);
        key_diff(key, guess_key);
#endif
    }

    printf("\n");
    printf("MCMC target: %lf\n", target);
    printf("Max target: %lf\n", target_max);
    score_update(plain, text_score, text_len);
    target = target_cal(text_score, train_score, text_len, train_len);
    printf("Plain target: %lf\n", target);
    decrypt(recover, cipher, guess_key, text_len);
    text_diff(plain, recover, text_len);
    key_diff(key, max_key);
}
