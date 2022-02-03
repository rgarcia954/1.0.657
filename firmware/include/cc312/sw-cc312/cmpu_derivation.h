/***************************************************************************
* The confidential and proprietary information contained in this file may
* only be used by a person authorised under and to the extent permitted
* by a subsisting licensing agreement from ARM Limited or its affiliates.
*   (C) COPYRIGHT [2001-2017] ARM Limited or its affiliates.
*       ALL RIGHTS RESERVED
* This entire notice must be reproduced on all copies of this file
* and copies of this file may only be made by a person if such person is
* permitted to do so under the terms of a subsisting license agreement
* from ARM Limited or its affiliates.
*****************************************************************************/

#ifndef _CMPU_DERIVATION_H
#define _CMPU_DERIVATION_H


uint32_t CC_PROD_Derivation_Instantiate (uint32_t *pEntrSrc,
                                         uint32_t  sourceSize,
                                         uint8_t *pKey,
                                         uint8_t *pIv);
uint32_t CC_PROD_Derivation_Generate(uint8_t *pKey,
                                     uint8_t *pIv,
                                     uint32_t *pOutputBuff,
                                     uint32_t  outDataSize);

#endif  //_CMPU_DERIVATION_H

