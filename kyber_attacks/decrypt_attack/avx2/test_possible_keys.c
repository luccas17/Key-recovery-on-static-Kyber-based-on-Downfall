#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "params.h"
#include "indcpa.h"
#include "randombytes.h"

int main()
{
    uint8_t pk[KYBER_PUBLICKEYBYTES];
    uint8_t sk[KYBER_INDCPA_SECRETKEYBYTES];
    uint8_t ct[KYBER_CIPHERTEXTBYTES];
    uint8_t m_test[KYBER_SSBYTES];
    uint8_t m_dec[KYBER_SSBYTES];
    FILE *public_key_file;
    FILE *possible_secret_keys_file;

    for (int i = 0; i < KYBER_SSBYTES; i++)
    {
        m_test[i] = i;
    }

    uint8_t kr[2 * KYBER_SYMBYTES];
    randombytes(kr, KYBER_SYMBYTES);

    if ((public_key_file = fopen("public_key.txt", "rb")) == NULL)
    {
        printf("Error! opening public key file");
        exit(1);
    }

    if ((possible_secret_keys_file = fopen("possible_secret_keys.txt", "rb")) == NULL)
    {
        printf("Error! opening secret keys file file");
        exit(1);
    }

    fread(pk, KYBER_PUBLICKEYBYTES, 1, public_key_file);

    indcpa_enc(ct, m_test, pk, kr + KYBER_SYMBYTES);

    int is_equal = 1;
    while (fread(sk, KYBER_INDCPA_SECRETKEYBYTES, 1, possible_secret_keys_file))
    {
        indcpa_dec(m_dec, ct, sk);
        is_equal = 1;
        for (int i = 0; i < KYBER_SSBYTES; i++)
        {
            is_equal = is_equal && m_test[i] == m_dec[i];
        }
        if (is_equal)
            break;
    }

    printf("Tested message: ");
    for (int i = 0; i < KYBER_SSBYTES; i++)
    {
        printf("%02x", m_test[i]);
    }
    printf("\nDecrypted message: ");
    for (int i = 0; i < KYBER_SSBYTES; i++)
    {
        printf("%02x", m_dec[i]);
    }
    printf("\n");
    if (is_equal)
    {
        printf("\nSecret key:\n");
        for (int i = 0; i < 12 * KYBER_K; i++)
        {
            if (i % 6 == 0)
                printf("------ %d -------\n", i / 6);
            for (int j = 0; j < 4; j++)
            {
                for (int k = 0; k < 8; k++)
                {
                    printf("%02x", sk[((i * 4) + j) * 8 + k]);
                }
                printf(" ");
            }
            printf("\n");
        }
    }

    fclose(public_key_file);
    fclose(possible_secret_keys_file);

    return 0;
}