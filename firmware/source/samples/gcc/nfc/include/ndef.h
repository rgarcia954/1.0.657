/**
 * @ndef.h
 * @brief Header file for NDEF payload information
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
#ifndef NDEF_H_
#define NDEF_H_

static uint8_t RAW_ARRAY[64] = { 0x11, 0x22, 0x33, 0x44,    /* UID 0:3 */
                                 0xB1, 0x20, 0x00, 0x07,    /* UID 4:7 */
                                 0x08, 0x09,              /* UID 8:9 */
                                 0xFF, 0xFF,              /* Lock 0:1 tag read only */
                                 0xE1, 0x10, 0x06, 0x0F,    /* CC0 0xE1=>NDEF message; CC1 0x10=>Version number 1.0; CC2
                                                             * 0x06=>Data memory size/8 (in this case 6*8=48bytes); CC3
                                                             * 4MSB read access granted, 4LSB write acces not granted */
                                 0x03, 0x0F,              /* TLV Tag=> NDEF message 0x03; TLV Length => Length of NDEF
                                                           * message; */
                                 0xD1,                    /* NDEF message: record */
                                 0x01,                    /* NDEF message: Type length */
                                 0x0B,                    /* NDEF message: Payload length */
                                 0x55,                    /* NDEF message: Type */
                                 0x01, 0x6F, 0x6E, 0x73,    /* NDEF message: Payload */
                                 0x65, 0x6D, 0x69, 0x2E,    /* NDEF message: Payload */
                                 0x63, 0x6F, 0x6D,        /* NDEF message: Payload */
                                 0xFE,                    /* TLV Terminator */
                                 0x00, 0x00, 0x00, 0x00,    /* Memory fill */
                                 0x00, 0x00, 0x00, 0x00,    /* Memory fill */
                                 0x00, 0x00, 0x00, 0x00,    /* Memory fill */
                                 0x00, 0x00, 0x00, 0x00,    /* Memory fill */
                                 0x00, 0x00, 0x00, 0x00,    /* Memory fill */
                                 0x00, 0x00, 0x00, 0x00,    /* Memory fill */
                                 0x00, 0x00, 0x00, 0x00,    /* Memory fill */
                                 0x00, 0x00                 /* Memory fill */
};

#endif    /* NDEF_H_ */
