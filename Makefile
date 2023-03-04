CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic $(shell pkg-config --cflags gmp)
LFLAGS = $(shell pkg-config --libs gmp)

all: keygen encrypt decrypt

keygen: keygen.o rsa.o randstate.o numtheory.o
	$(CC) -o $@ $^ $(LFLAGS)

encrypt: encrypt.o rsa.o randstate.o numtheory.o
	$(CC) -o $@ $^ $(LFLAGS)

decrypt: decrypt.o rsa.o randstate.o numtheory.o
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f keygen encrypt decrypt *.o
	rm -f en_stdin_file en_stdout_file
	rm -f de_stdin_file de_stdout_file

cleankeys:
	#rm -f *.{pub,priv}
	rm -f *.pub
	rm -f *.priv

format:
	clang-format -i -style=file *.[ch]
