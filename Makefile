target=mcmc
CC = gcc
CFLAGS = -std=c99 -lm -O2 -Wall -D KEY_LEN=128
SOURCES = $(wildcard src/*.cc)
SCORES = $(wildcard score/*)

all: $(target)

mcmc: mcmc_stream_cipher.c stream_cipher.h stream_cipher.c
	$(CC) $(CFLAGS) mcmc_stream_cipher.c stream_cipher.c -o mcmc_stream_cipher

demo: mcmc_stream_cipher.c stream_cipher.h stream_cipher.c
	$(CC) $(CFLAGS) -D DEMO mcmc_stream_cipher.c stream_cipher.c -o mcmc_stream_cipher
