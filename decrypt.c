// clang-format off
#include <stdio.h>
#include <assert.h>
#include <gmp.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

// clang-format on

gmp_randstate_t state;

int main(int argc, char **argv) { // allows compiled file to get command line
                                  // args (usually its void)
  randstate_init(69420);

  int opt = 0;

  // setting default values

  // bool trackers to check if we are going to be using stdin and stdout for
  // input/output
  int input_stdin = 1;
  int output_stdout = 1;

  char *input_file_name = (char *)(calloc(sizeof(char), 4096));
  char *output_file_name = (char *)(calloc(sizeof(char), 4096));
  char *pv_file_name = (char *)(calloc(sizeof(char), 4096));

  // if file pointers return null

  if (input_file_name == NULL) {
    fprintf(stderr, "No more memory!\n");
    return 1;
  }

  if (output_file_name == NULL) {
    fprintf(stderr, "No more memory!\n");
    return 1;
  }

  if (pv_file_name == NULL) {
    fprintf(stderr, "No more memory!\n");
    return 1;
  }

  strcpy(pv_file_name, "rsa.priv");

  int verbose = 0;

  // while loop to read getopt command line args
  while ((opt = getopt(argc, argv, "i:o:n:vh")) != -1) {
    switch (opt) {
    case 'i': // input file name
      strcpy(input_file_name, optarg);
      input_stdin = 0;
      break;
    case 'o': // setting output file name
      strcpy(output_file_name, optarg);
      output_stdout = 0;
      break;

    case 'n': // public key file name
      strcpy(pv_file_name, optarg);
      break;

    case 'v': // verbose
      verbose = 1;
      break;

    case 'h': // help message
      fprintf(stderr, "Usage: ./decrypt [options]\n");
      fprintf(stderr, "  ./decrypt decrypts an input file using the specified "
                      "private key file,\n");
      fprintf(stderr, "  writing the result to the specified output file.\n");
      fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: "
                      "standard input.\n");
      fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: "
                      "standard output.\n");
      fprintf(stderr, "    -n <keyfile>: Private key is in <keyfile>. Default: "
                      "rsa.priv.\n");
      fprintf(stderr, "    -v          : Enable verbose output.\n");
      fprintf(stderr,
              "    -h          : Display program synopsis and usage.\n");

      free(pv_file_name);
      free(output_file_name);
      free(input_file_name);

      return 0;
    default: // if the user has an invalid option, print help message and return
             // a non zero exit code
      fprintf(stderr, "Usage: ./decrypt [options]\n");
      fprintf(stderr, "  ./decrypt decrypts an input file using the specified "
                      "private key file,\n");
      fprintf(stderr, "  writing the result to the specified output file.\n");
      fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: "
                      "standard input.\n");
      fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: "
                      "standard output.\n");
      fprintf(stderr, "    -n <keyfile>: Private key is in <keyfile>. Default: "
                      "rsa.priv.\n");
      fprintf(stderr, "    -v          : Enable verbose output.\n");
      fprintf(stderr,
              "    -h          : Display program synopsis and usage.\n");
      free(pv_file_name);
      free(output_file_name);
      free(input_file_name);
      return 1;
    }
  }

  mpz_t n;
  mpz_init(n);
  mpz_t d;
  mpz_init(d);

  FILE *input_file;
  FILE *output_file;
  FILE *pv_file;

  if (input_stdin == 0) { // if the user specified an input file
    input_file = fopen(input_file_name, "r");
    if (input_file == NULL) {
      fprintf(stderr,
              "decrypt: Couldn't open %s to read ciphertext: No such file or "
              "directory\n",
              input_file_name);
      return 1;
    }
  } else {
    input_file = stdin;
  }

  if (output_stdout == 0) { // if the user specified an output file
    output_file = fopen(output_file_name, "w");
  } else {
    output_file = stdout;
  }

  // reading the private key
  pv_file = fopen(pv_file_name, "r");

  if (pv_file ==
      NULL) { // if the pv_file pointer returns null, the file doesn't exist in
              // the directory or theres another error
    fprintf(stderr, "./decrypt: couldn't open %s to read private key.\n",
            pv_file_name);
    return 1;
  }

  rsa_read_priv(n, d, pv_file); // values for n,e,s,username should be filled

  if (verbose == 1) { // if verbose is on
    gmp_fprintf(stderr, "n - modulus (%zu bits): %Zd\n", mpz_sizeinbase(n, 2),
                n);
    gmp_fprintf(stderr, "d - private exponent (%zu bits): %Zd\n",
                mpz_sizeinbase(d, 2), d);
  }

  rsa_decrypt_file(input_file, output_file, n, d); // decrypting the input_file

  mpz_clear(d);
  mpz_clear(n);

  fclose(pv_file); // closing the private key file

  // we cannot close stdin and stdout, so we have to check if the input and
  // output were stdin or stdout
  if (input_stdin == 0) {
    fclose(input_file);
  }
  if (output_stdout == 0) {
    fclose(output_file);
  }

  free(input_file_name);
  free(pv_file_name);
  free(output_file_name);

  randstate_clear();
  return 0;
}
