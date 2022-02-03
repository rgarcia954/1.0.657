/**
 * @file AES_ECB_vectors.h
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
#ifndef AES_ECB_VECTORS_H_
#define AES_ECB_VECTORS_H_

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

/**
 * @brief Define the size of the plain text in bytes
 */
#define AES_ECB_PLAIN_SIZE    64


/**
 * @brief Define the plain text for the AES-ECB
 */
static const uint8_t AES_ECB_PLAIN_TEXT[] = {
        0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96, 
        0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A, 
        0xAE, 0x2D, 0x8A, 0x57, 0x1E, 0x03, 0xAC, 0x9C, 
        0x9E, 0xB7, 0x6F, 0xAC, 0x45, 0xAF, 0x8E, 0x51, 
        0x30, 0xC8, 0x1C, 0x46, 0xA3, 0x5C, 0xE4, 0x11, 
        0xE5, 0xFB, 0xC1, 0x19, 0x1A, 0x0A, 0x52, 0xEF, 
        0xF6, 0x9F, 0x24, 0x45, 0xDF, 0x4F, 0x9B, 0x17, 
        0xAD, 0x2B, 0x41, 0x7B, 0xE6, 0x6C, 0x37, 0x10
    };
    
/**
 * @brief Define the key size for the ECB-AES128
 */
#define AES_ECB_128_KEY_SIZE    128

/**
 * @brief Define the key for the ECB-AES128
 */
static const uint8_t AES_ECB_128_KEY[] = {
        0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 
        0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
    };

/**
 * @brief Define the expected cipher for the ECB-AES128
 */
static const uint8_t AES_ECB_128_CIPHER[] = {
        0x3A, 0xD7, 0x7B, 0xB4, 0x0D, 0x7A, 0x36, 0x60, 
        0xA8, 0x9E, 0xCA, 0xF3, 0x24, 0x66, 0xEF, 0x97, 
        0xF5, 0xD3, 0xD5, 0x85, 0x03, 0xB9, 0x69, 0x9D, 
        0xE7, 0x85, 0x89, 0x5A, 0x96, 0xFD, 0xBA, 0xAF, 
        0x43, 0xB1, 0xCD, 0x7F, 0x59, 0x8E, 0xCE, 0x23, 
        0x88, 0x1B, 0x00, 0xE3, 0xED, 0x03, 0x06, 0x88, 
        0x7B, 0x0C, 0x78, 0x5E, 0x27, 0xE8, 0xAD, 0x3F, 
        0x82, 0x23, 0x20, 0x71, 0x04, 0x72, 0x5D, 0xD4
    };
    
/**
 * @brief Define the key size for the ECB-AES192
 */
#define AES_ECB_192_KEY_SIZE    192

/**
 * @brief Define the key for the ECB-AES192
 */
static const uint8_t AES_ECB_192_KEY[] = {
        0x8E, 0x73, 0xB0, 0xF7, 0xDA, 0x0E, 0x64, 0x52, 
        0xC8, 0x10, 0xF3, 0x2B, 0x80, 0x90, 0x79, 0xE5, 
        0x62, 0xF8, 0xEA, 0xD2, 0x52, 0x2C, 0x6B, 0x7B
    };

/**
 * @brief Define the expected cipher for the ECB-AES192
 */
static const uint8_t AES_ECB_192_CIPHER[] = {
        0xBD, 0x33, 0x4F, 0x1D, 0x6E, 0x45, 0xF2, 0x5F, 
        0xF7, 0x12, 0xA2, 0x14, 0x57, 0x1F, 0xA5, 0xCC, 
        0x97, 0x41, 0x04, 0x84, 0x6D, 0x0A, 0xD3, 0xAD, 
        0x77, 0x34, 0xEC, 0xB3, 0xEC, 0xEE, 0x4E, 0xEF, 
        0xEF, 0x7A, 0xFD, 0x22, 0x70, 0xE2, 0xE6, 0x0A, 
        0xDC, 0xE0, 0xBA, 0x2F, 0xAC, 0xE6, 0x44, 0x4E, 
        0x9A, 0x4B, 0x41, 0xBA, 0x73, 0x8D, 0x6C, 0x72, 
        0xFB, 0x16, 0x69, 0x16, 0x03, 0xC1, 0x8E, 0x0E
    };
    
/**
 * @brief Define the key size for the ECB-AES256
 */
#define AES_ECB_256_KEY_SIZE    256

/**
 * @brief Define the key for the ECB-AES256
 */
static const uint8_t AES_ECB_256_KEY[] = {
        0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE, 
        0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81, 
        0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7, 
        0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4
    };

/**
 * @brief Define the expected cipher for the ECB-AES256
 */
static const uint8_t AES_ECB_256_CIPHER[] = {
        0xF3, 0xEE, 0xD1, 0xBD, 0xB5, 0xD2, 0xA0, 0x3C, 
        0x06, 0x4B, 0x5A, 0x7E, 0x3D, 0xB1, 0x81, 0xF8, 
        0x59, 0x1C, 0xCB, 0x10, 0xD4, 0x10, 0xED, 0x26, 
        0xDC, 0x5B, 0xA7, 0x4A, 0x31, 0x36, 0x28, 0x70, 
        0xB6, 0xED, 0x21, 0xB9, 0x9C, 0xA6, 0xF4, 0xF9, 
        0xF1, 0x53, 0xE7, 0xB1, 0xBE, 0xAF, 0xED, 0x1D, 
        0x23, 0x30, 0x4B, 0x7A, 0x39, 0xF9, 0xF3, 0xFF, 
        0x06, 0x7D, 0x8D, 0x8F, 0x9E, 0x24, 0xEC, 0xC7
    };
    
/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif /* AES_ECB_VECTORS_H_ */