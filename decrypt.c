#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gmp.h>
#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

#define OPTIONS "i:o:n:vh"

void print_help_menu() {
	fprintf(stderr, "Usage: ./decrypt [options]\n  ./decrypt decrypts an input file using the specified private key file,\n  writing the result to the specified output file.\n    -i <infile> : Read input from <infile>. Default: standard input.\n    -o <outfile>: Write output to <outfile>. Default: standard output.\n    -n <keyfile>: Private key is in <keyfile>. Default: rsa.priv.\n    -v          : Enable verbose output.\n    -h          : Display program synopsis and usage.\n");
}

int main (int argc, char **argv) {
        int opt = 0;
	char *default_infile = "de_stdin_file";
	char *infile = default_infile;
	char *default_outfile = "de_stdout_file";
	char *outfile = default_outfile;
	char *default_privkey = "rsa.priv";
	char *privkey = default_privkey;
	int verbose = 0;
	mpz_t n, d;
	mpz_inits(n, d, NULL);

        while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
                switch (opt) {
                        case 'i':
				// replace file name for infile
				infile = optarg;
                                break;
                        case 'o':
				// replace file name for outfile
				outfile = optarg;				
				break;
                        case 'n':
				// replace file name for priv key file
				privkey = optarg;
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
	FILE *fppriv;
	FILE *fp_infile;
	FILE *fp_outfile;
	fppriv = fopen(privkey, "r+");

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

	//Check if files opened correctly
	if (fppriv == NULL) {
		fprintf(stderr, "./decrypt: couldn't open privfile to read private key.\n");
		print_help_menu();
		return 1;
	}
	if (fp_infile == NULL) {
		fprintf(stderr, "decrypt: Couldn't open %s to read ciphertext: No such file or directory\n", infile);
		print_help_menu();
		return 1;
	}
	if (fp_outfile == NULL) {
		//Should never occur because its opening with w+ and creating a newfile
		fprintf(stderr, "Error opening file: %s\n", outfile);
		return 1;
	}

	//Read the encrypted data from infile and decrypt it into outfile
	rsa_read_priv(n, d, fppriv);
	if (verbose == 1) {
		gmp_fprintf(stderr, "n - modulus (%lu bits): %Zu\nd - private exponent (%lu bits): %Zu\n", mpz_sizeinbase(n, 2), n, mpz_sizeinbase(d, 2), d);
	}
	rsa_decrypt_file(fp_infile, fp_outfile, n, d);
	
        //Print temp stdout file contents to the standard output if it was used
        rewind(fp_outfile);
	if (outfile == default_outfile) {
                while (fscanf(fp_outfile, "%c", &word) != EOF) {
                        //Print to stdout	
                        printf("%c", word);
                }
        }

	fclose(fppriv);
	fclose(fp_infile);
	fclose(fp_outfile);
	mpz_clears(n, d, NULL);
	return 0;
}
