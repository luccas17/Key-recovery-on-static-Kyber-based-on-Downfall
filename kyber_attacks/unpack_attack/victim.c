#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>

extern void nttfrombytes_avx(__m256i *r, const uint8_t *a, const __m256i *qdata);

#define ALIGNED_INT16(N)            \
    union                           \
    {                               \
        int16_t coeffs[N];          \
        __m256i vec[(N + 15) / 16]; \
    }
typedef ALIGNED_INT16(256) poly;

typedef struct
{
    poly vec[4];
} polyvec;

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define ARRAY_SIZE 384 * 4

uint8_t hex_to_uint8(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return 0; // Invalid character
}

void hex_string_to_uint8_array(const char *hex_string, uint8_t *array)
{
    size_t length = strlen(hex_string);
    if (length % 2 != 0)
    {
        printf("Invalid hex string length\n");
        return;
    }
    if (length / 2 != ARRAY_SIZE)
    {
        printf("Hex string must be %d characters long\n", ARRAY_SIZE * 2);
        return;
    }

    for (size_t i = 0; i < ARRAY_SIZE; i++)
    {
        uint8_t byte = (hex_to_uint8(hex_string[2 * i]) << 4) |
                       hex_to_uint8(hex_string[2 * i + 1]);
        array[i] = byte;
    }
}

void polyvec_frombytes(polyvec *r, const uint8_t a[384])
{
    poly qdata;
    unsigned int i;
    for (i = 0; i < 4; i++)
        nttfrombytes_avx((&r->vec[i])->vec, a + i * 384, qdata.vec);
}

void unpack_sk()
{
    polyvec skpv;
    uint8_t sk[4 * 384];

    const char *hex_string = "ab547f6e51a78f76851eb8398b415a72a07e3fc729232ca258359f10b919b4889742999271eabdc00097597b5418eb9f0a025377bb20f5d0a43aa4b0aec063c1629874540d69811fd4caad4de9224e66849d13b5cae938def926069a1d8cf6cfaccb3c4f0c34e0da54c0617fa1a4c5d27544f495249e139f0b35b267a94f2bfb8b18011d233114d806a2ba52b84cc2cec6905a503527c4a22e5a5234868c5f2166bffd8aaaf7b95871e7a96ad9a7fe342a0f16444a5943ae810ccf995ac541947e5b09c70591de553dbe3a861fc49f71a2cc700777ffe7c276064c2bd26fa47a82edb78bd465634b1a43aca9a14da1392e2a10faf89c0d7a0f611233a8160890969f5a7833e9f9cce418b6450180aa5729fa91acbb2197df4c44bcb92f5c4926a8a4cce331bc161234487ab4a99c34c5a0aebda4982360be09da3b6cd9aa29813db5f79bdb4950d5c2ab3ff3ac12f0c8ade8bd7a24836301a4ee248738c64aea28c5e28ca99ef3b173956d5a3173a0e63cfa377686686788914dccd9b59e7504739c8a9c37c5b3ea2e791ba73590129fbc7861b70cc7b0c6494b0e9f0cc1be4359f5d42286f83d9a9887ce5c35dbfcc81bcc22892a429207587384b7382c7b269307d066ba34405203e9665cf14225ccaefbfb5ba2394768f396bcd35e5f0571bfebaa6dd1ac77111610c535095c7254f189b8f58e9314839d5c8f1927c11eb732092126bcda16ffa6836421b9e751697c2a166d264b5cb93e9ae35c28e58d02037c9146b3bee6bfeeba041b6789f21a52035b2c70b42a33952bc7c2c87a99bf91f0ad498c64bd2025c015a3842b36381a17dd4c8542b59560cb82a5e0c0dde04ff4a03e121b2546aca721f4c050c658b1379d8577b3a4f2004f1721a6fb63acec7390e3b04873517136c5145156553a289501182a03662a22825cc0b87d069ca4327bd2cb57ca72119e237e08db477a2718f1c94114869702eaad012538d22b4f1ec4362b191a9b583237287fc3cb648ea6b4c330ad82f32c0b3483f4b0a7fb82c2693c9a7d279a8f09422833096fca4b7f8168d06a0da2c52004fb00db82a6cde8917e92069ba29b5e944a9b33348a226d3de8924d48325c5a3250fc3faf3285f39523ac8bccd7eba7c811c5fc7c43283c5bd84a78e6c210b1832993792d12468547284a90f73898899eaa107d45e1652138792f398fc844c169f8738e075b2ab91a0d065c10d7c73070ccbce3485832be4fea6ee570025f6a5c44fb3113c63ffc92635708c930b8ab47aabb07074a5468a8c17b6bcde921ae437b3aab498e762c398aaf4495c2d859bf2743b5f038b604da43adb4ceb11b4637420f8fdca2da1b58e700b2ed166e907548a27c977b0843b361a5b5375792f3a72e290dc2327ccbda7d820a097ee44c4000588b29a75c37afa7422ae0484935f5be59833edd421939bc3b6f22a8b4b34c00c5535fcb29016345e23b97912809f80a8de0e165904212007d6c6b87b51e4bc7f484905b46a8c955cdd612b79e37c79f868cd49467a420b8f717cab69140e777953ca82113f574834b45449b50f887b945d7c5cfb0b96b94ce33ea5f4bdbc8c292c0dc0c3094d68e7b445b6eb4254bf3c46ff49ae843cb2806c424b697a343330cfa03cc5aaaccd9219af03675a072b82a279f053b8ceb3773796d4e97b03bf87e6904b3c46202bda0602f0c67f8a803461a85e3d67191eb37a7b93a9b32a37f5b59a112c61fea8405715daf372e198a0bae7b5adcfb522df08fdde3b71cb34eab9a82f4daa3d86b146ad6776d7a9bc1442007f7787b817fcb96766451538126790031b979056ffdebb0a2abacc1343cfafc4bded81163674d31eb36a29343b489113df552b6b5106013a0d2a83dda30555389114a3058d0bbb8c7c915e5f3a0ebf7512a2c4101556075c74892b95f195b200e1c492053c87ce700bed4c7d021a12b049616a96b2bf33e75206041110740c1ba692cb1a308a8dca976b369bf7d6b23cfb92b4b180356d23473231148b353b150ae4b98305be90ebad886caac0e0842c75c7923cb1c71c056b537697eb2f6bec5684e951c667607b2dab26d4b1007b38c45ca906da2408a938285b529288a3a84bf284dd0e4b6ccd9aaccf1289deb649b829a8700772a7a916fa96d468870ad354c";
    hex_string_to_uint8_array(hex_string, sk);

    // for (int i = 0; i < 4 * 384; i++)
    // {
    //     sk[i] = (1 + i + (i / 256)) % 256;
    // }
    for (int i = 0; i < 48; i++)
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

    while (1)
    {
        polyvec_frombytes(&skpv, sk);
    }
}

void main()
{

    unpack_sk();
}
