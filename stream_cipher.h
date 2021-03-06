//#define KEY_LEN 1024
#define BETA 1

typedef char* text_t;
typedef char* key_t;
typedef double** score_t;

void key_init(key_t key);
void key_print(key_t key);
void encrypt(text_t plaintext, text_t ciphertext, key_t key, int text_len);
void decrypt(text_t recovertext, text_t ciphertext, key_t key, int text_len);
void text_print(text_t text, int text_len);
void score_update(text_t text, score_t score, int text_len);
void score_print(score_t score);
double target_cal(score_t text_score, score_t train_score, int text_len, int train_len);
void text_diff(text_t plaintext, text_t recovertext, int text_len);
void key_diff(key_t key, key_t guess_key);
double key_avg_diff(key_t key, key_t guess_key);
