#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <tgmath.h>
#include "stream_cipher.h"

void key_init(key_t key) {
    // generate kry in random
    for(int i = 0; i < KEY_LEN; i++)
        key[i] = rand() % 128;
}

void key_print(key_t key) {
    printf("key: \n  ");
    for(int i = 0; i < KEY_LEN; i++)
        printf("%02x", key[i]);
    printf("\n");
}

void encrypt(text_t plaintext, text_t ciphertext, key_t key, int text_len) {
    for(int i = 0; i < text_len; i++) {
        ciphertext[i] = ((int)plaintext[i] + (int)key[i % KEY_LEN]) % 128;
    }
    ciphertext[text_len] = 0;
}

void decrypt(text_t recovertext, text_t ciphertext, key_t key, int text_len) {
    for(int i = 0; i < text_len; i++) {
        recovertext[i] = ((int)ciphertext[i] + (int)(128 - key[i % KEY_LEN])) % 128;
    }
}

void text_print(text_t text, int text_len) {
    for(int i = 0; i < text_len; i++)
        printf("%02x", text[i]);
    printf("\n");
}

void score_update(text_t text, score_t score, int text_len) {
    for(int i = 0; i < 128; i++) {
        for(int j = 0; j < 128; j++) 
            score[i][j] = 0;
    }
    for(int i = 1; i < text_len; i++) {
        score[text[i-1]][text[i]]++;
    }
}

void score_print(score_t score) {
    for(int i = 0; i < 128; i++) {
        for(int j = 0; j < 128; j++)
            printf("%.3lf ", score[i][j]);
        printf("\n");
    }
}

double target_cal(score_t text_score, score_t train_score, int text_len, int train_len) {
    double target = 0;
    for(int i = 0; i < 128; i++) {
        for(int j = 0; j < 128; j++) {
            target += abs(train_score[i][j] - text_score[i][j]*train_len/text_len);
        }
    }
    target = 2 * train_len - target;
    target = target / train_len * 10;
    return pow(2.718, target);
}

void text_diff(text_t plaintext, text_t recovertext, int text_len) {
    int cnt = 0;
    for(int i = 0; i < text_len; i++)
        cnt += (recovertext[i] == plaintext[i]);
    printf("[Text]\n  Matched: %d out of %d\n", cnt, text_len);
}

void key_diff(key_t key, key_t guess_key) {
    int cnt = 0;
    for(int i = 0; i < KEY_LEN; i++)
        cnt += (key[i] == guess_key[i]);
    printf("[Key]\n  Matched: %d out of %d avg: %lf\n", cnt, KEY_LEN, key_avg_diff(key, guess_key));
}

double key_avg_diff(key_t key, key_t guess_key) {
    double cnt = 0;
    for(int i = 0 ; i < KEY_LEN; i++)
        cnt += abs(key[i] - guess_key[i]);
    return cnt / KEY_LEN;
}

