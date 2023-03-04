#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gmp.h>
#include <time.h>
#include <sys/stat.h>
#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

#define OPTIONS "b:i:n:d:s:vh"

void print_help_menu(void) {
	fprintf(stderr, "Usage: ./keygen [options]\n  ./keygen generates a public / private key pair, placing the keys into the public and private\n  key files as specified below. The keys have a modulus (n) whose length is specified in\n  the program options.\n    -s <seed>   : Use <seed> as the random number seed. Default: time()\n    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n    -v          : Enable verbose output.\n    -h          : Display program synopsis and usage.\n");
}

int main (int argc, char **argv) {
	int opt = 0;
	uint64_t min_bits = 1024;
	uint64_t m_rabin_iters = 50;
	char *default_pbfile = "rsa.pub";
	char *pubfile = default_pbfile;
	char *default_pvfile = "rsa.priv";
	char *privfile = default_pvfile;
	uint64_t seed = time(NULL);
	int verbose = 0;
	mpz_t p, q, e, n, priv_key, user, signature;
	mpz_inits(p, q, e, n, priv_key, user, signature, NULL);
	
	while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
		switch (opt) {
			case 'b':
				min_bits = atoi(optarg);
				/*if (min_bits < 50 || min_bits > 4096) {
					fprintf(stderr, "./keygen: Number of bits must be 50-4096, not %lu.\n", min_bits);
					print_help_menu();
					return 1;
				}*/
				break;
			case 'i':
				m_rabin_iters = atoi(optarg);
				break;
			case 'n':
				//replace filename of public file
				pubfile = optarg;
				break;
			case 'd':
				//replace filename of private file
				privfile = optarg;
				break;
			case 's':
				seed = atoi(optarg);
				break;
			case 'v':
				verbose = 1;
				break;
			case 'h':
				//print help menu
				print_help_menu();	
				return 0;
			default:
				print_help_menu();
				return 1;
		}
	}
	
	FILE *fppub;
	FILE *fppriv; 
	fppub = fopen(pubfile, "w+");
	fppriv = fopen(privfile, "w+");

	//Check if both files opened correctly, if not print name of file
	if (fppub == NULL) {
		//Should never occur because its opening with w+ and creating a newfile
		fprintf(stderr, "Error opening file '%s'\n", pubfile);
		return 1;
	} 
	if (fppriv == NULL) {
		//Should never occur because its opening with w+ and creating a newfile
		fprintf(stderr, "Error opening file '%s'\n", privfile);
		return 1;
	}

	//Change and check file permissions for private file
	int check_permissions = fchmod(fileno(fppriv), 0600);
	if (check_permissions != 0) {
		fprintf(stderr, "File permissions generated error\n");
		return 1;
	}
	
	//Set up values by creating them
	randstate_init(seed);
	rsa_make_pub(p, q, n, e, min_bits, m_rabin_iters);	
	rsa_make_priv(priv_key, e, p, q);
	char *user_name = getenv("USER");
	mpz_set_str(user, user_name, 62);
	gmp_printf("mpz_user: %Zu\n", user);
	rsa_sign(signature, user, priv_key, n);
	
	//Write these created values into public and private key
	rsa_write_pub(n, e, signature, user_name, fppub);
	rsa_write_priv(n, priv_key, fppriv);

	//print values to standard error
	if (verbose == 1) {
		gmp_fprintf(stderr, "username: %s\nuser signature (%lu bits): %Zu\np (%lu bits): %Zu\nq (%lu bits): %Zu\nn - modulus (%lu bits): %Zu\ne - public exponent (%lu bits): %Zu\nd - private exponent (%lu bits): %Zu\n", user_name, mpz_sizeinbase(signature, 2), signature, mpz_sizeinbase(p, 2), p, mpz_sizeinbase(q, 2), q, mpz_sizeinbase(n, 2), n, mpz_sizeinbase(e, 2), e, mpz_sizeinbase(priv_key, 2), priv_key);
	}
	
	fclose(fppub);
	fclose(fppriv);
	mpz_clears(p, q, e, n, priv_key, user, signature, NULL);
	randstate_clear();
	return 0;
}
