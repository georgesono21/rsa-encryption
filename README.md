CSE13s Fall

Assignment 5

George Sono (ysono)


Description of Files:
 - decrypt.c: contains implementation and main function for decrypt program
 - encrypt.c: contains implementation and main function for encrypt program
 - keygen.c: contains implementation and main function for keygen program
 - numtheory.c: contains implementation of number theory functions
 - nuntheory.h: specifies interface for functions in numtheory.c
 - randstate.c: contains implementation of random state interface for rsa.c and numtheory.c functions
 - randstate.h: specifies interface for clearing and initializing random state
 - rsa.c: contains the implmentation of RSA library functions
 - rsa.h: specifies the interface for functions in rsa.c
 - WRITEUP.pdf: writeup report on how code was tested
 - DESIGN.pdf: contains the pseudocode implementations of RSA, numtheory, decrypt, encrypt and keygen files and functions
 - README.md: contains the sources used, as well as file descriptions and instructions on how to run program (what you are reading currently)
 - Makefile: used to aid the compilation linking process of aforementioned files

 	
Command Line Options:
 
 decrypt.c Command Line Options:
  - i {infile} : Read input from infile. Default: standard input.
  - o {outfile}: Write output to outfile. Default: standard output.
  - n {keyfile}: Private key is in keyfile. Default: rsa.priv.
  - v          : Enable verbose output.
  - h          : Display program synopsis and usage.

 encrypt.c Command Line Options:
  - i {infile} : Read input from infile. Default: standard input.
  - o {outfile}: Write output to outfile. Default: standard output.
  - n {keyfile}: Public key is in keyfile. Default: rsa.pub.
  - v          : Enable verbose output.
  - h          : Display program synopsis and usage.
 
 keygen.c Command Line Options:
  - s {seed}   : Use {seed} as the random number seed. Default: time()
  - b {bits}   : Public modulus n must have at least {bits} bits. Default: 1024
  - i {iters}  : Run {iters} Miller-Rabin iterations for primality testing. Default: 50
  - n {pbfile} : Public key file is pbfile. Default: rsa.pub
  - d {pvfile} : Private key file is pvfile. Default: rsa.priv
  - v          : Enable verbose output.
  - h          : Display program synopsis and usage.
 
 Instructions on how to run:
  1. Download files into a directory
  2. Open the CLI for that directory
  3. Enter the command "make all" (the executables for keygen, decrypt, encrypt should show up)
  4. Enter the command "./keygen {options}" to create your public/private keys
  5. Create a plain text file that has the message you want to encrypt
  6. Create an empty cipher text file for the encrypted text 
  7. Enter the command "./encrypt -i {plain text file} -o {empty cipher text file}"
  8. Create an empty decrypted text file for the deciphered text
  9. Enter the command "./decrypt -i {cipher text file} -o {empty decrypted text file}"
  10. Now open the decrypted text file. It should be the same as your original plain text file if the encryption/decryption was successful.
  
  
Sources of help:
 - Nishant Khanorkar (TA) - asked questions and received help regarding make_pub functions and how signatures work
 - Zackary Jorquera (TA): asked questions received advice regarding Miller Rabin algorithn (calculating S and R) and also using fread and fwrite 
 - Sanjana Patil (TA): asked questions regarding the gmp_urandomb functions and how they are used in make_prime and also about how nbits worked and how random() is used in make_pub
 - Jessie Srinivas (Tutor): explained how the encrypt and block arrays worked and how import mpz functions can take in an array
 - Lev Teytelman (Tutor): asked questions and checked approach regarding variables inputted for mpz_export and import statements in encrypt_file and decrypt_file
 - Fabrice Kurmann (Tutor): asked questions and received advice regarding the block size in decrypt file and encrypt file
 - Ben Grant (Tutor): Followed his approach on how to generate a number within a given range (from his message on discord) with generating pbits in make_pub
 - Miles Glapa-Grossklag - Helped with clang format regarding headers not working properly.
 
 Websites that I used for help (mainly for learning filo i/o):
 
 Setting file pointers to stdin/stdout
 - Link: https://stackoverflow.com/questions/18505530/how-to-set-a-file-variable-to-stdout
 - How I used it: 
 From the stack overflow article, I learned how to set file pointers to stdin and stdout. I used these methods for encrypt and decrypt where the default arguments for input and output are standard input and standard output.
 
 Freeing dynamically allocating strings in C
 - Link: //https://stackoverflow.com/questions/10063222/freeing-strings-in-c
 - How I used it:
 When I was trying to free my dynamically allocated array of characters (for storing the username), I kept on running into an invalid pointer error. Searching the error message on Google, I found that it was because I was changing the pointer address. Meaning the method that I used to store the username actually changed the pointer's address. From the article, I learned that I should use strcpy() to keep the pointer address the same.


