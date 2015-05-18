target=mcmc train test
CC = gcc
CFLAGS = -std=c99 -lm -O2 -Wall
SOURCES = $(wildcard src/*.cc)
SCORES = $(wildcard score/*)

all: $(target)

mcmc: mcmc_stream_cipher.c stream_cipher.h stream_cipher.c
	$(CC) $(CFLAGS) mcmc_stream_cipher.c stream_cipher.c -o mcmc_stream_cipher

train: train.c train_result.c
	$(CC) $(CFLAGS) train.c -o train
	$(CC) $(CFLAGS) train_result.c -o train_result

train_run:
	for i in $(SOURCES); do\
		(./train $$i $$i.score);\
	done

ratio: 
	./train_result $(SCORES)

test:
	$(CC) $(CFLAGS) test.c -o test

