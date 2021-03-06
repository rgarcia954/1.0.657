/**
 * @file SHA384_vectors.h
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
#ifndef SHA384_VECTORS_H_
#define SHA384_VECTORS_H_

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
static const uint8_t SHA384_1_DIGEST[] __attribute__((aligned(16))) = {
        0xCB, 0x00, 0x75, 0x3F, 0x45, 0xA3, 0x5E, 0x8B, 
        0xB5, 0xA0, 0x3D, 0x69, 0x9A, 0xC6, 0x50, 0x07, 
        0x27, 0x2C, 0x32, 0xAB, 0x0E, 0xDE, 0xD1, 0x63, 
        0x1A, 0x8B, 0x60, 0x5A, 0x43, 0xFF, 0x5B, 0xED, 
        0x80, 0x86, 0x07, 0x2B, 0xA1, 0xE7, 0xCC, 0x23, 
        0x58, 0xBA, 0xEC, 0xA1, 0x34, 0xC8, 0x25, 0xA7
    };

/**
 * @brief Define the fields structure for sample 1
 */
static const tSHAFields SHA384_1 = {
    .digestLength  = 48,
    .digest        = SHA384_1_DIGEST,
    .messageLength = 3,
    .message       = (const unsigned char *) "abc"
};

/**
 * @brief Define the initial digest for sample 2
 */
static const uint8_t SHA384_2_DIGEST[] __attribute__((aligned(16))) = {
        0x09, 0x33, 0x0C, 0x33, 0xF7, 0x11, 0x47, 0xE8, 
        0x3D, 0x19, 0x2F, 0xC7, 0x82, 0xCD, 0x1B, 0x47, 
        0x53, 0x11, 0x1B, 0x17, 0x3B, 0x3B, 0x05, 0xD2, 
        0x2F, 0xA0, 0x80, 0x86, 0xE3, 0xB0, 0xF7, 0x12, 
        0xFC, 0xC7, 0xC7, 0x1A, 0x55, 0x7E, 0x2D, 0xB9, 
        0x66, 0xC3, 0xE9, 0xFA, 0x91, 0x74, 0x60, 0x39
    };

/**
 * @brief Define the fields structure for sample 2
 */
static const tSHAFields SHA384_2 = {
    .digestLength  = 48,
    .digest        = SHA384_2_DIGEST,
    .messageLength = 112,
    .message       = (const unsigned char *) "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"
};



static const pSHAFields SHA384_TABLES[] = {
    (const pSHAFields) &SHA384_1,
    (const pSHAFields) &SHA384_2,
    (const pSHAFields) NULL
};

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif /* SHA384_VECTORS_H_ */
