#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <gmp.h>
#include "randstate.h"

extern gmp_randstate_t state;

void gcd(mpz_t d, mpz_t a, mpz_t b) {
	mpz_t t, atmp, btmp;
	mpz_inits(t, atmp, btmp, NULL);
	mpz_set(atmp, a);
	mpz_set(btmp, b);
	while (mpz_cmp_ui(btmp, 0) != 0) {
		mpz_set(t, btmp);
		mpz_mod(btmp, atmp, btmp);
		mpz_set(atmp, t);
	}
	mpz_set(d, atmp);
	mpz_clears(t, atmp, btmp, NULL);
	return;
}
 
void mod_inverse(mpz_t o, mpz_t a, mpz_t n) {
	mpz_t r, r1, t, t1, q, mult, aux;
	mpz_inits(r, r1, t, t1, q, mult, aux, NULL);
	mpz_set(r, n);
	mpz_set(r1, a);
	mpz_set_ui(t, 0);
	mpz_set_ui(t1, 1);

	while (mpz_cmp_ui(r1, 0) != 0) {
		mpz_fdiv_q(q, r, r1);

		mpz_set(aux, r);
		mpz_set(r, r1);
		mpz_mul(mult, q, r1);
		mpz_sub(r1, aux, mult);

		mpz_set(aux, t);
		mpz_set(t, t1);
		mpz_mul(mult, q, t1);
		mpz_sub(t1, aux, mult);
	}
	if (mpz_cmp_ui(r, 1) > 0) {
		mpz_set_ui(o, 0);
		mpz_clears(r, r1, t, t1, q, mult, aux, NULL);
		return;
	}
	if (mpz_cmp_ui(t, 0) < 0) {
		mpz_add(t, t, n);
	}
	mpz_set(o, t);
	mpz_clears(r, r1, t, t1, q, mult, aux, NULL);
	return;
}

void pow_mod(mpz_t o, mpz_t a, mpz_t d, mpz_t n) {
	mpz_t v, p, dtmp, tmpMul, odd;
	mpz_inits(v, p, dtmp, tmpMul, odd, NULL);
	mpz_set_ui(v, 1);
	mpz_set(dtmp, d);
	mpz_set(p, a);
	while (mpz_cmp_ui(dtmp, 0) > 0) {
		mpz_mod_ui(odd, dtmp, 2);
		if (mpz_cmp_ui(odd, 0) != 0) {
			mpz_mul(tmpMul, v, p);
			mpz_mod(v, tmpMul, n);
		}
		mpz_mul(tmpMul, p, p);
		mpz_mod(p, tmpMul, n);	
		mpz_fdiv_q_ui(dtmp, dtmp, 2);
	}
	mpz_set(o, v);
	mpz_clears(v, p, dtmp, tmpMul, odd, NULL);
	return;
}

bool is_prime(mpz_t n, uint64_t iters) {
	if (mpz_cmp_ui(n, 0) == 0 || mpz_cmp_ui(n, 2) == 0) {
		return 0;
	}
	if (mpz_cmp_ui(n, 1) == 0 || mpz_cmp_ui(n, 3) == 0) {
		return 1;
	}

	mpz_t s, r, a, j, t, y, nsub1, ssub1, nsub3;
	mpz_inits(s, r, a, j, t, y, nsub1, ssub1, nsub3, NULL);
	mpz_sub_ui(nsub1, n, 1);
	mpz_sub_ui(ssub1, s, 1);
	mpz_sub_ui(nsub3, n, 3);
	mpz_set_ui(t, 2);
	for (mpz_set(r, nsub1); mpz_odd_p(r)==0; mpz_add_ui(s, s, 1)) {
		mpz_fdiv_q_ui(r, r, 2);
	}
	
	for (uint64_t i = 1; i<iters; i+=1) {
		//choose random a (2<a<n-2)
		mpz_urandomm(a, state, nsub3);
		mpz_add_ui(a, a, 2);
		
		pow_mod(y, a, r, n);
		if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, nsub1) != 0) {
			mpz_set_ui(j, 1);
			
			while (mpz_cmp(j, ssub1) <= 0 && mpz_cmp(y, nsub1) != 0) {
				pow_mod(y, y, t, n);
				if (mpz_cmp_ui(y, 1) == 0) {
					mpz_clears(s, r, a, j, t, y, nsub1, ssub1, NULL);
					return 0;
				}
				mpz_add_ui(j, j, 1);
				if (mpz_cmp(y, nsub1) != 0) {
					mpz_clears(s, r, a, j, t, y, nsub1, ssub1, NULL);
					return 0;
				}
			}
		}	
	}
	mpz_clears(s, r, a, j, t, y, nsub1, ssub1, NULL);
	return 1;
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
	int cont = 0;
	mpz_urandomb(p, state, bits);
	do {	
		if (is_prime(p, iters) == 0) {
			mpz_urandomb(p, state, bits);
			continue;
		}
		if (mpz_sizeinbase(p,2) < bits) {
			mpz_urandomb(p, state, bits);
			continue;
		}
		cont = 1;
	}
	while (cont != 1);

	return;
}
