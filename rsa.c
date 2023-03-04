#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "numtheory.h"

extern gmp_randstate_t state; 

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
	uint64_t first_bits;
	uint64_t last_bits;
	mpz_t lambda_n, totient, divisor, rand, check, psub1, qsub1;
	mpz_inits(lambda_n, totient, divisor, rand, check, psub1, qsub1, NULL);

	do {
		first_bits = random() % (2*nbits/4);
		first_bits += nbits/4;
		last_bits = nbits - first_bits;
		make_prime(p, first_bits, iters);
		make_prime(q, last_bits, iters);
		mpz_mul(n, p, q);
	} while (mpz_sizeinbase(n,2) < nbits);
	mpz_sub_ui(psub1, p, 1);
	mpz_sub_ui(qsub1, q, 1);
	mpz_mul(totient, psub1, qsub1);
	gcd(divisor, psub1, qsub1);
	mpz_fdiv_q(lambda_n, totient, divisor);
	gmp_printf("mkpub: lambda_n: %Zu\n", lambda_n);

	while (1) {
		mpz_urandomb(rand, state, nbits);
		gcd(check, rand, lambda_n);
		if (mpz_cmp_ui(check, 1) == 0) {	
			break;
		}
	}
	mpz_set(e, rand);
	mpz_clears(lambda_n, totient, divisor, rand, check, psub1, qsub1, NULL);
	return;
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
	gmp_fprintf(pbfile, "%Zx\n%Zx\n%Zx\n%s\n", n, e, s, username);	
	return;
}

void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
	gmp_fscanf(pbfile, "%Zx\n%Zx\n%Zx\n%s\n", n, e, s, username);
	return;
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
	mpz_t lambda_n, totient, divisor, psub1, qsub1;
	mpz_inits(lambda_n, totient, divisor, psub1, qsub1, NULL);
	mpz_sub_ui(psub1, p, 1);
	mpz_sub_ui(qsub1, q, 1);
	mpz_mul(totient, psub1, qsub1);
	gcd(divisor, psub1, qsub1);
	mpz_fdiv_q(lambda_n, totient, divisor);	
	mod_inverse(d, e, lambda_n);
	mpz_clears(lambda_n, totient, divisor, psub1, qsub1, NULL);
	return;
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
	gmp_fprintf(pvfile, "%Zx\n%Zx\n", n, d);
	return;
}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
	gmp_fscanf(pvfile, "%Zx\n%Zx\n", n, d);
	return;
}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
	pow_mod(c, m, e, n);
	return;
}

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
	mpz_t c, message_data;
	mpz_inits(c, message_data, NULL);
	uint64_t j;
	size_t n_size = mpz_sizeinbase(n, 2);
	uint64_t k = (n_size-1) / 8;
	
	void *ptr1 = calloc(k, sizeof(uint8_t));
	uint8_t *blocks = (uint8_t*) ptr1;
	void *ptr2 = calloc(k-1, sizeof(uint8_t));
	uint8_t *buffer = (uint8_t*) ptr2;
	blocks[0] = 0xFF;
	while((j=fread(buffer, sizeof(uint8_t), k-1, infile)) != 0) {
		//J is number of bytes actually read
		for (uint64_t i=1; i<=j; i+=1) {
			blocks[i] = buffer[i-1];
		}
		if (j != k-1) {
			mpz_import(message_data, j+1, 1, sizeof(uint8_t), 1, 0, blocks);
		} else { 
			mpz_import(message_data, k, 1, sizeof(uint8_t), 1, 0, blocks);
		}
		rsa_encrypt(c, message_data, e, n);	
		gmp_fprintf(outfile, "%Zx\n", c);
	}
	mpz_clears(c, message_data, NULL);
	free(ptr1);
	free(ptr2);
	return;
}

void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
	pow_mod(m, c, d, n);
	return;
}

void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
	mpz_t c, message_data;
	mpz_inits(c, message_data, NULL);
	size_t n_size = mpz_sizeinbase(n, 2); 
	uint64_t k = (n_size-1) / 8;
	size_t j = 0;	

	void *m_ptr = calloc(k, sizeof(uint8_t));
	uint8_t *blocks = (uint8_t*) m_ptr;
	while (gmp_fscanf(infile, "%Zx\n", c) != EOF) {
		rsa_decrypt(message_data, c, d, n);
		mpz_export(blocks, &j, 1, sizeof(uint8_t), 1, 0, message_data);
		fwrite(blocks+1, sizeof(uint8_t), j-1, outfile);
	}
	
	mpz_clears(c, message_data, NULL);
	free(m_ptr);
	return;
}

void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
	pow_mod(s, m, d, n);
	return;
}

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
	mpz_t t;
	mpz_init(t);	
	pow_mod(t, s, e, n);
	gmp_printf("t: %Zu ==? m: %Zu\n", t, m);
	if (mpz_cmp(m, t) == 0) {
		mpz_clear(t);
		return 1;
	}
	mpz_clear(t);
	return 0;
}
