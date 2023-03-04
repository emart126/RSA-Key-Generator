### Asignment 5 README

# keygen.c

./keygen generates a public / private key pair, 
placing the keys into the public and private key files 
as specified below. The keys have a modulus (n) whose 
length is specified in the program options.

Instructions:
use the command 'make' to generate the files needed
to run this program and use the 'make clean' command
to remove unwanted files from the directory once the
proram is not in use.
The command to run this executable is ./keygen [options]

Options:
-    -s <seed>   : Use <seed> as the random number seed. Default: time()
-    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024
-    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50
-    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub
-    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv
-    -v          : Enable verbose output.
-    -h          : Display program synopsis and usage.

# encrypt.c

./encrypt encrypts an input file using the 
specified public key file, writing the result to 
the specified output file.

Instructions:
The command to run this executable is ./encrypt [options]
If the user doesn't use the -i option, the user will have to type 
in their own message to be encrypted. When the user is done typing
their message they should use lshift+d to terminate their message.

Options:
-    -i <infile> : Read input from <infile>. Default: standard input.
-    -o <outfile>: Write output to <outfile>. Default: standard output.
-    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.
-    -v          : Enable verbose output.
-    -h          : Display program synopsis and usage.

# decrypt.c

./decrypt decrypts an input file using the 
specified private key file, writing the result to
the specified output file.

Instructions:
The command to run this executable is ./decrypt [options]
If the user doesn't use the -i option, the user will have to type
in their own encrypted data. When the user is done typing
their data, they should use lshift+d to terminate the standard inputi.

Options:
-    -i <infile> : Read input from <infile>. Default: standard input.
-    -o <outfile>: Write output to <outfile>. Default: standard output.
-    -n <keyfile>: Private key is in <keyfile>. Default: rsa.priv.
-    -v          : Enable verbose output.
-    -h          : Display program synopsis and usage.

# generation-script.sh

./generation-script.sh tests the keygen program several times
doing a couple different things to test if output is successfull.
The script outputs what iteration it is currently on in terms of bits,
then in terms of seed.
The script is not relevant to the functionality of the code.

Instructions:
The command to run this executable is ./generation-script.sh
