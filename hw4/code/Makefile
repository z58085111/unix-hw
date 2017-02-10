CC=gcc
OMP = -fopenmp
PT = -lpthread
CRYPT= -lcrypt

all: decrypt

decrypt: decrypt.c
	$(CC) decrypt.c -std=gnu99 -o $@ $(CRYPT) $(PT)

omp-decrypt: decrypt.c
	$(CC) decrypt_omp.c -std=gnu99 -o $@ $(CRYPT) $(OMP)

clean:
	rm decrypt omp-decrypt
