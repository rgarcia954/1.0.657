/**
 * @nfc_api.c
 * @brief C file for NFC related A.P.I
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

#include "hw.h"
#include "api_isohfllhw.h"
#include "platform_config.h"
#include "iso14443.h"
#include "nfc_api.h"
#include "ndef.h"
static inline uint8_t _isohf_getHFIORAMbyte_local(uint8_t offset)
{
    return ((uint8_t *)(HF_IO_RAM_START_ADD))[offset];
}

static inline void _isohf_setHFIORAMbyte_local(uint8_t offset, uint8_t data)
{
    ((uint8_t *)(HF_IO_RAM_START_ADD))[offset] = data;
}

/* Type A config tables in RAM */
static uint8_t Layer3Source[HF_IO_RAM_INIT_ISOALAYER3];

/* Type A config tables in RAM */
static uint8_t ats[20];          /* value to be sent */

uint32_t _LLHW_isohf_compareIORAM2Mem_local(HFCTRL isohf,  uint8_t *pComp, uint32_t byte_size)
{
    for (uint32_t i = 0; i < byte_size; i++)
    {
        if (pComp[i] != _isohf_getHFIORAMbyte_local(i))
        {
            return -1;
        }
    }
    return 0;
}

void _LLHW_isohf_copyMem2IORAM_local(HFCTRL isohf, uint8_t *pSource, uint8_t offset, uint32_t bytesLength)
{
    for (uint32_t i = 0; i < bytesLength; i++)
    {
        _isohf_setHFIORAMbyte_local(offset + i, (uint8_t)pSource[i]);
    }
}

void _LLHW_isohf_configIORAM4TypeALayer3_local(HFCTRL isohf, uint8_t *Layer3Source)
{
    _LLHW_isohf_copyMem2IORAM_local(isohf,
                                    Layer3Source,
                                    (uint8_t)(HF_IO_RAM_EMPTY_OFFSET >> 2),
                                    HF_IO_RAM_INIT_ISOALAYER3);
}

void _isohf_configTypeALayer3BootAndWait_local(HFCTRL isohf, uint8_t *Layer3Source)
{
    /* Fill the RAM for Layer 3 */
    _LLHW_isohf_configIORAM4TypeALayer3_local(isohf, Layer3Source);

    /* Wait */
    _LLHW_isohf_waitForRx(isohf, 0x0);
}

uint8_t read_block(uint8_t *ptr_resp, uint8_t *ptr_data)
{
    uint16_t start_index = 0;
    uint16_t end_index = 0;
    uint8_t j = 0;
    start_index = ptr_data[1] * 4;          /* offset from command */
    end_index = start_index + 16;           /* offset + 16bytes = end index */
    for (uint8_t i = start_index; i < end_index; i++)    /* copy all data from start index to end_index */
    {
        ptr_resp[j++] = RAW_ARRAY[i];
    }
    return j;
}

void NFC_Initialize(void)
{
    /* Configure NFC peripheral */
    SYSCTRL->NFC_CFG = NFC_EN;
    _isohf_setProtocolWP(HFCTRL_IP, 0xFF00);
    _isohf_setReboundFilterCfg(HFCTRL_IP, 0x7);
    _isohf_setDigitalCnt2Cfg(HFCTRL_IP, 0x0);

    /* Enable interrupts for NFC*/
    _isohf_enableEndOfComIt(HFCTRL_IP);
    NVIC_EnableIRQ(NFC_IRQn);

    /* Setup anti collision response */
    Layer3Source[0] = 0x44;    /* ATQA first byte */
    Layer3Source[1] = 0x00;    /* ATQA second byte */
    Layer3Source[2] = RAW_ARRAY[0];    /* UID0 */
    Layer3Source[3] = RAW_ARRAY[1];    /* UID1 */
    Layer3Source[4] = RAW_ARRAY[2];    /* UID2 */
    Layer3Source[5] = RAW_ARRAY[3];    /* UID3 */
    Layer3Source[6] = RAW_ARRAY[4];    /* UID4 */
    Layer3Source[7] = RAW_ARRAY[5];    /* UID5 */
    Layer3Source[8] = RAW_ARRAY[6];    /* UID6 */
    Layer3Source[9] = RAW_ARRAY[7];    /* UID7 */
    Layer3Source[10] = RAW_ARRAY[8];    /* UID8 */
    Layer3Source[11] = RAW_ARRAY[9];    /* UID9 */
    Layer3Source[12] = 0x0F;          /* SAK NOT COMP */
    Layer3Source[13] = 0x00;          /* SAK OK (This byte indicates if compatible or not with ISO 14443-4, in this case
                                       * not compatible) */

    /* Waiting for the end of the boot if the boot triggers the HF */
    _isohf_setProtocolUID(HFCTRL_IP, 1);
    _LLHW_isohf_waitUntilPlatformHand(HFCTRL_IP);

    /* IO RAM configuration for Layer 3 */
    _isohf_configTypeALayer3BootAndWait_local(HFCTRL_IP, &Layer3Source[0]);
}

void NFC_Task(void)
{
    uint8_t *ptr = (uint8_t *)PLATFORM_HF_BUFFER_ADDR;

    _LLHW_isohf_waitUntilPlatformHand(HFCTRL_IP);

    /* check no error in the RX frame */
    uint8_t val = ((_isohf_getStatus(HFCTRL_IP) & 0xF0) == 0) ? *ptr : 0;

    /* if HLTA */
    if ((val & 0xFF) == 0x50)
    {
        _LLHW_isohf_waitForRx(HFCTRL_IP, HF_P_CTRL_BACK2HALT);
    }

    /* Read command */
    else if (((val & 0xFF) == 0x30))
    {
        uint8_t size = read_block(ats, ptr);
        _LLHW_isohf_copyMem2IORAM_local(HFCTRL_IP, &ats[0], 0, size);
        _LLHW_isohf_launchTx(HFCTRL_IP, 0, 3, size, 0);
    }
    else if (((val & 0xFF) == 0x55))
    {
        uint8_t size = _isohf_getRxFrameSize(HFCTRL_IP) - 2;
        ptr[0] = 0x66;
        _LLHW_isohf_launchTx(HFCTRL_IP, 0, 3, size, 0);
    }

    /* Write Command */
    else if ((val & 0xFF) == 0xA2)
    {
        /* Prepare and send response
         * NACK not done, need to respond only 4 bits */
        _LLHW_isohf_waitForRx(HFCTRL_IP, 0x0);
    }

    /* Select block */
    else if ((val & 0xFE) == 0x02)
    {
        /* Mute
         * Not implemented, small memory */
        _LLHW_isohf_waitForRx(HFCTRL_IP, 0x0);
    }
    else
    {
        /* Mute */
        _LLHW_isohf_waitForRx(HFCTRL_IP, HF_P_CTRL_BACK2HALT);
    }
}

void NFC_IRQHandler(void)
{
    _isohf_clearEndOfComStatus(HFCTRL_IP);
}
