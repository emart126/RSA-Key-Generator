#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gmp.h>
#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

#define OPTIONS "i:o:n:vh"

void print_help_menu(void) {
	fprintf(stderr, "Usage: ./encrypt [options]\n  ./encrypt encrypts an input file using the specified public key file,\n  writing the result to the specified output file.\n    -i <infile> : Read input from <infile>. Default: standard input.\n    -o <outfile>: Write output to <outfile>. Default: standard output.\n    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n    -v          : Enable verbose output.\n    -h          : Display program synopsis and usage.\n");
}

int main (int argc, char **argv) {
        int opt = 0;
	char *default_infile = "en_stdin_file";
	char *infile = default_infile;
	char *default_outfile = "en_stdout_file";
	char *outfile = default_outfile;
	char *default_pubkey = "rsa.pub";
	char *pubkey = default_pubkey;
	char user_name[] = "temp strng";
	int verbose = 0;
	mpz_t n, e, signature, user;
	mpz_inits(n, e, signature, user, NULL);

        while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
                switch (opt) {
                        case 'i':
				// replace file name of infile
				infile = optarg;
                               break;
                        case 'o':
				// replace file name of outfile
				outfile = optarg;
                                break;
                        case 'n':
				// replace file name of public key
				pubkey = optarg;
                                break;
                        case 'v':
				verbose = 1;
				break;
                        case 'h':
				// print help menu
				print_help_menu();
                                return 0;
			default:
				print_help_menu();
				return 1;
                }
        }
	FILE *fppub;
	FILE *fp_infile;
	FILE *fp_outfile;
	fppub = fopen(pubkey, "r");
	
	char word;
	if (infile == default_infile) {
		FILE *tmp_stdin = fopen(default_infile, "w+");
		while (scanf("%c", &word) != EOF) {	
			fprintf(tmp_stdin, "%c", word);
		}
		fclose(tmp_stdin);
	}
	fp_infile = fopen(infile, "r+");
	fp_outfile = fopen(outfile, "w+");

	//check if files opened correctly
	if (fppub == NULL) {
		fprintf(stderr, "./encrypt: couldn't open pubfile to read public key.\n");
		print_help_menu();
		return 1;
	}
	if (fp_infile == NULL) {
		fprintf(stderr, "encrypt: Couldn't open %s to read plaintext: No such file or directory\n", infile);
		print_help_menu();
		return 1;
	}
	if (fp_outfile == NULL) {
		//Should never occur because its opening with w+ and creating a newfile
		fprintf(stderr, "Error opening file '%s'\n", outfile);
		return 1;
	}
	
	//Read public key file and verify it has the correct signature	
	rsa_read_pub(n, e, signature, user_name, fppub);
	mpz_set_str(user, user_name, 62);
	gmp_printf("mpz_user: %Zu\n", user);
	if (verbose == 1) {	
		gmp_fprintf(stderr, "username: %s\nuser signature (%lu bits): %Zu\nn - modulus (%lu bits): %Zu\ne - public exponent (%lu bits): %Zu\n", user_name, mpz_sizeinbase(signature, 2), signature, mpz_sizeinbase(n, 2), n, mpz_sizeinbase(e, 2), e);
	}
	if (rsa_verify(user, signature, e, n) == 0)
	{
		//rsa_verify does not have the correct functionality
		printf("Verification error\n");
		return 1;
	}
	
	//Encrypt the data, put it into outfile
	rsa_encrypt_file(fp_infile, fp_outfile, n, e);

	//Print temp stdout file contents to the standard output if it was used
	rewind(fp_outfile);
	if (outfile == default_outfile) {	
		while (fscanf(fp_outfile, "%c", &word) != EOF) {
			//Print to stdout
			printf("%c", word);
		}
	}

	fclose(fppub);
	fclose(fp_infile);
	fclose(fp_outfile);
	mpz_clears(n, e, signature, user, NULL);
	return 0;
}
