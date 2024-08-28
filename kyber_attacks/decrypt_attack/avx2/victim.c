#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "params.h"
#include "indcpa.h"
#include "randombytes.h"

#define PRINTMODE 1
FILE *file;

int main(void){

    uint8_t pk[KYBER_PUBLICKEYBYTES];
    uint8_t sk[KYBER_SECRETKEYBYTES];
    uint8_t ct[KYBER_CIPHERTEXTBYTES];
    uint8_t m_before[KYBER_SSBYTES];
    uint8_t m_after[KYBER_SSBYTES];

    indcpa_keypair(pk, sk);
    
    uint8_t m[100*(KYBER_PUBLICKEYBYTES)];
    uint8_t msk[KYBER_SECRETKEYBYTES];
    for(int i = 0; i < 100; i++){
        indcpa_keypair(&m[i*(KYBER_PUBLICKEYBYTES)],msk);
    }


    #if PRINTMODE == 1
        file = fopen("output.txt", "w");

        fprintf(file, "pk: ");
        for(int i = 0; i < KYBER_PUBLICKEYBYTES; i++){
            fprintf(file, "%02x",pk[i]);
        }

        fprintf(file, "\nsk: ");
        for(int i = 0; i < KYBER_INDCPA_SECRETKEYBYTES; i++){
            fprintf(file, "%02x",sk[i]);
        }
        
        // Write bytes to file to test decryption in "test_possible_keys"
        FILE *pkfile = fopen("public_key.txt", "wb+");
        for(int i=0; i < sizeof(pk); i++){
            fwrite(&pk[i], 1, sizeof(pk[i]), pkfile);
        }
        fclose(pkfile);
        FILE *skfile = fopen("possible_secret_keys.txt", "ab+");
        for(int i=0; i < KYBER_INDCPA_SECRETKEYBYTES; i++){
            fwrite(&sk[i], 1, sizeof(sk[i]), skfile);
        }
        fclose(skfile);
        
    #endif

    uint8_t kr[2*KYBER_SYMBYTES];
    randombytes(kr, KYBER_SYMBYTES);

    for(int i = 0; i < KYBER_SSBYTES; i++){
        m_before[i] = i;
    }
    #if PRINTMODE == 1

        fprintf(file, "\nmessagebefore: ");
        for(int i = 0; i < KYBER_SSBYTES; i++){
            fprintf(file, "%02x", m_before[i]);
        }
    #endif
    
    indcpa_enc(ct, m_before, pk, kr+KYBER_SYMBYTES);

    #if PRINTMODE == 1

        fprintf(file, "\nciphertext: ");
        for(int i = 0; i < KYBER_CIPHERTEXTBYTES; i++){
            fprintf(file, "%02x", ct[i]);
        }

    #endif

    for(int i = 0; i < 12*KYBER_K; i ++)
    {
        if(i % 6 == 0) printf("------ %d -------\n",i/6);
        for (int j = 0; j < 4; j ++)
        {
            for (int k = 0; k < 8; k++){
                printf("%02x", sk[((i*4)+j)*8+k]);
            }
            printf(" ");
        }
        printf("\n");
    }

    fclose(file);

    int chunk = 0;
    while(1){
        chunk += KYBER_CIPHERTEXTBYTES;
        indcpa_dec(m_after, &m[chunk % (100*(KYBER_PUBLICKEYBYTES))], sk);
    }


    return 0;
}