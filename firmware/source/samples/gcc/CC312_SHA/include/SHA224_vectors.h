/**
 * @file SHA224_vectors.h
 *
 * @copyright @parblock
 * Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee.
 *
 * This is Reusable Code.
 * @endparblock
 */
#ifndef SHA224_VECTORS_H_
#define SHA224_VECTORS_H_

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/*
 * Autogenerated Test Vectors - Do Not Edit
 */


#ifndef _SHA_STRUCTURE_DEFINED_
#define _SHA_STRUCTURE_DEFINED_

typedef struct {
    uint32_t            digestLength;
    uint32_t            messageLength;
    const uint8_t       *digest;
    const unsigned char *message;
} tSHAFields, *pSHAFields;

#endif /* _SHA_STRUCTURE_DEFINED_ */ 

/**
 * @brief Define the initial digest for sample 1
 */
static const uint8_t SHA224_1_DIGEST[] __attribute__((aligned(16))) = {
        0x23, 0x09, 0x7D, 0x22, 0x34, 0x05, 0xD8, 0x22, 
        0x86, 0x42, 0xA4, 0x77, 0xBD, 0xA2, 0x55, 0xB3, 
        0x2A, 0xAD, 0xBC, 0xE4, 0xBD, 0xA0, 0xB3, 0xF7, 
        0xE3, 0x6C, 0x9D, 0xA7
    };

/**
 * @brief Define the fields structure for sample 1
 */
static const tSHAFields SHA224_1 = {
    .digestLength  = 28,
    .digest        = SHA224_1_DIGEST,
    .messageLength = 3,
    .message       = (const unsigned char *) "abc"
};

/**
 * @brief Define the initial digest for sample 2
 */
static const uint8_t SHA224_2_DIGEST[] __attribute__((aligned(16))) = {
        0x75, 0x38, 0x8B, 0x16, 0x51, 0x27, 0x76, 0xCC, 
        0x5D, 0xBA, 0x5D, 0xA1, 0xFD, 0x89, 0x01, 0x50, 
        0xB0, 0xC6, 0x45, 0x5C, 0xB4, 0xF5, 0x8B, 0x19, 
        0x52, 0x52, 0x25, 0x25
    };

/**
 * @brief Define the fields structure for sample 2
 */
static const tSHAFields SHA224_2 = {
    .digestLength  = 28,
    .digest        = SHA224_2_DIGEST,
    .messageLength = 56,
    .message       = (const unsigned char *) "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
};



static const pSHAFields SHA224_TABLES[] = {
    (const pSHAFields) &SHA224_1,
    (const pSHAFields) &SHA224_2,
    (const pSHAFields) NULL
};

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif /* SHA224_VECTORS_H_ */
